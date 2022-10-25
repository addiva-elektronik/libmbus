//------------------------------------------------------------------------------
// Copyright (C) 2012, Robert Johansson, Raditex AB
// All rights reserved.
//
// rSCADA
// http://www.rSCADA.se
// info@rscada.se
//
//------------------------------------------------------------------------------

#include <getopt.h>
#include <string.h>

#include <stdio.h>
#include <mbus/mbus.h>

static int debug = 0;
static char *arg0;

int
usage(int rc)
{
        fprintf(stderr,
		"usage: %s [-d] [-b BAUDRATE] [-r RETRIES] device mbus-address [file]\n"
		"    optional flag -d for debug printout\n"
		"    optional argument file for file. if omitted read from stdin\n", arg0);
        return rc;
}

int
main(int argc, char **argv)
{
    unsigned char raw_buff[4096], buff[4096];
    char *device, *addr_str, *file = NULL;
    long baudrate = 9600;
    size_t buff_len, len;
    int address, result;
    FILE *fp = NULL;
    int retries = 0;
    long port;
    int c;

    mbus_frame reply, request;
    mbus_frame_data reply_data;
    mbus_handle *handle = NULL;

    arg0 = argv[0];
    memset(&reply, 0, sizeof(mbus_frame));
    memset(&reply_data, 0, sizeof(mbus_frame_data));

    while ((c = getopt(argc, argv, "b:dr:")) != EOF) {
	    switch (c) {
	    case 'b':
		    baudrate = atol(optarg);
		    break;
	    case 'd':
		    debug = 1;
		    break;
	    case 'r':
		    retries = atoi(optarg);
		    break;
	    default:
		    return usage(0);
	    }
    }

    if (optind >= argc)
	    return usage(1);
    
    device = argv[optind++];
    if (optind >= argc)
	    return usage(1);
    addr_str = argv[optind++];
    if (optind < argc)
	    file = argv[optind++];

    if ((handle = mbus_context_serial(device)) == NULL)
    {
        fprintf(stderr, "Could not initialize M-Bus context: %s\n",  mbus_error_str());
        return 1;
    }
    
    if (debug)
    {
        mbus_register_send_event(handle, &mbus_dump_send_event);
        mbus_register_recv_event(handle, &mbus_dump_recv_event);
    }

    if (mbus_connect(handle) == -1)
    {
        fprintf(stderr, "Failed to setup connection to M-bus gateway\n%s\n", mbus_error_str());
        return 1;
    }

    if (mbus_serial_set_baudrate(handle, baudrate) == -1)
    {
        fprintf(stderr,"Failed to set baud rate.\n");
        return 1;
    }

    if (mbus_is_secondary_address(addr_str))
    {
        // secondary addressing
        int ret;

        ret = mbus_select_secondary_address(handle, addr_str);

        if (ret == MBUS_PROBE_COLLISION)
        {
            fprintf(stderr, "%s: Error: The address mask [%s] matches more than one device.\n", __func__, addr_str);
            return 1;
        }
        else if (ret == MBUS_PROBE_NOTHING)
        {
            fprintf(stderr, "%s: Error: The selected secondary address does not match any device [%s].\n", __func__, addr_str);
            return 1;
        }
        else if (ret == MBUS_PROBE_ERROR)
        {
            fprintf(stderr, "%s: Error: Failed to select secondary address [%s].\n", __func__, addr_str);
            return 1;
        }
        address = MBUS_ADDRESS_NETWORK_LAYER;
    }
    else
    {
        // primary addressing
        address = atoi(addr_str);
    }

    //
    // read hex data from file or stdin
    //
    if (file != NULL)
    {
        if ((fp = fopen(file, "r")) == NULL)
        {
            fprintf(stderr, "%s: failed to open '%s'\n", __func__, file);
            return 1;
        }
    }
    else
    {
        fp = stdin;
    }

    memset(raw_buff, 0, sizeof(raw_buff));
    len = fread(raw_buff, 1, sizeof(raw_buff), fp);

    if (fp != stdin)
    {
        fclose(fp);
    }

    buff_len = mbus_hex2bin(buff,sizeof(buff),raw_buff,sizeof(raw_buff));

    //
    // attempt to parse the input data
    //
    result = mbus_parse(&request, buff, buff_len);
    if (result < 0)
    {
        fprintf(stderr, "mbus_parse: %s\n", mbus_error_str());
        return 1;
    }
    else if (result > 0)
    {
        fprintf(stderr, "mbus_parse: need %d more bytes\n", result);
        return 1;
    }

    //
    // send the request
    //
    if (mbus_send_frame(handle, &request) == -1)
    {
        fprintf(stderr, "Failed to send mbus frame: %s\n", mbus_error_str());
        return 1;
    }

    //
    // this should be option:
    //
    if (mbus_recv_frame(handle, &reply) != MBUS_RECV_RESULT_OK)
    {
        fprintf(stderr, "Failed to receive M-Bus response frame: %s\n", mbus_error_str());
        return 1;
    }

    mbus_disconnect(handle);
    mbus_context_free(handle);
    return 0;
}

