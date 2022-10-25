//------------------------------------------------------------------------------
// Copyright (C) 2011, Robert Johansson, Raditex AB
// All rights reserved.
//
// rSCADA
// http://www.rSCADA.se
// info@rscada.se
//
//------------------------------------------------------------------------------

#include <string.h>

#include <stdio.h>
#include <mbus/mbus.h>

static int debug = 0;

//------------------------------------------------------------------------------
// Execution starts here:
//------------------------------------------------------------------------------
int
main(int argc, char **argv)
{
    char *host, *addr_mask = NULL;
    mbus_handle *handle = NULL;
    mbus_frame reply;
    int rc = 1;
    long port;

    memset((void *)&reply, 0, sizeof(mbus_frame));

    if (argc != 4 && argc != 3)
    {
        fprintf(stderr, "usage: %s host port [address-mask]\n", argv[0]);
        fprintf(stderr, "\trestrict the search by supplying an optional address mask on the form\n");
        fprintf(stderr, "\t'FFFFFFFFFFFFFFFF' where F is a wildcard character\n");
        return 0;
    }

    host = argv[1];
    port = atol(argv[2]);
    if (argc == 4)
    {
        addr_mask = argv[3];
    }
    else
    {
        addr_mask = "FFFFFFFFFFFFFFFF";
    }

    if ((port < 0) || (port > 0xFFFF))
    {
        fprintf(stderr, "Invalid port: %ld\n", port);
        return 1;
    }

    if (mbus_is_secondary_address(addr_mask) == 0)
    {
        fprintf(stderr, "Misformatted secondary address mask. Must be 16 character HEX number.\n");
        return 1;
    }

    if ((handle = mbus_context_tcp(host, port)) == NULL)
    {
        fprintf(stderr, "Could not initialize M-Bus context: %s\n", mbus_error_str());
        return 1;
    }

    if (mbus_connect(handle) == -1)
    {
        fprintf(stderr, "Failed to setup connection to M-bus gateway\n");
        goto err_connect;
    }

    //
    // init slaves
    //
    if (debug)
        printf("%s: debug: sending init frame #1\n", __func__);

    if (mbus_send_ping_frame(handle, MBUS_ADDRESS_NETWORK_LAYER, 1) == -1)
    {
	fprintf(stderr, "Failed sending ping to network layer address: %s\n", mbus_error_str());
        goto err;
    }

    //
    // resend SND_NKE, maybe the first get lost
    //
    if (debug)
        printf("%s: debug: sending init frame #2\n", __func__);

    if (mbus_send_ping_frame(handle, MBUS_ADDRESS_BROADCAST_NOREPLY, 1) == -1)
    {
	fprintf(stderr, "Failed sending ping to broadcast (no-reply) address: %s\n", mbus_error_str());
        goto err;
    }

    if (mbus_scan_2nd_address_range(handle, 0, addr_mask) == -1)
    {
	fprintf(stderr, "Failed secondary address scan: %s\n", mbus_error_str());
    }
    else
    {
	rc = 0;
    }

err:
    mbus_disconnect(handle);
err_connect:
    mbus_context_free(handle);

    return rc;
}

