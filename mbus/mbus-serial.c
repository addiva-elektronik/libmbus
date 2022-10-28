//------------------------------------------------------------------------------
// Copyright (C) 2011, Robert Johansson, Raditex AB
// All rights reserved.
//
// rSCADA
// http://www.rSCADA.se
// info@rscada.se
//
//------------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <limits.h>
#include <fcntl.h>

#include <sys/types.h>

#include <stdio.h>
#include <strings.h>

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#else
#error "No 'struct termios' on platform, you need to supply your own mbus-serial.c"
#endif
#include <errno.h>
#include <string.h>

#include "mbus-serial.h"
#include "mbus-protocol-aux.h"
#include "mbus-protocol.h"

#define PACKET_BUFF_SIZE 2048

//------------------------------------------------------------------------------
/// Set up a serial connection handle.
//------------------------------------------------------------------------------
int
mbus_serial_connect(mbus_handle *handle)
{
    mbus_serial_data *serial_data;
    const char *device;
    struct termios *term;

    if (handle == NULL)
        return -1;

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL || serial_data->device == NULL)
    {
        mbus_error_str_set("M-Bus serial handle missing tty data ptr.");
        return -1;
    }

    device = serial_data->device;
    term = &(serial_data->t);
    //
    // create the SERIAL connection
    //

    // Use blocking read and handle it by serial port VMIN/VTIME setting
    handle->fd = open(device, O_RDWR | O_NOCTTY);
    if (handle->fd == -1)
    {
        mbus_error_str_set("M-Bus serial failed opening tty %s, error %d", device, errno);
        return -1;
    }

    memset(term, 0, sizeof(*term));
    term->c_cflag |= (CS8|CREAD|CLOCAL);
    term->c_cflag |= PARENB;

    // No received data still OK
    term->c_cc[VMIN] = (cc_t) 0;

    return mbus_serial_set_baudrate(handle, 2400);
}

//------------------------------------------------------------------------------
// Set baud rate for serial connection
//------------------------------------------------------------------------------
int
mbus_serial_set_baudrate(mbus_handle *handle, long baudrate)
{
    speed_t speed;
    mbus_serial_data *serial_data;

    if (handle == NULL)
    {
        mbus_error_str_set("%s: invalid M-Bus handle.", __func__);
        return -1;
    }

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL)
    {
        mbus_error_str_set("M-Bus serial handle missing tty data ptr.");
        return -1;
    }

    // Wait at most 0.2 sec.Note that it starts after first received byte!!
    // I.e. if CMIN>0 and there are no data we would still wait forever...
    //
    // The specification mentions link layer response timeout this way:
    // The time structure of various link layer communication types is described in EN60870-5-1. The answer time
    // between the end of a master send telegram and the beginning of the response telegram of the slave shall be
    // between 11 bit times and (330 bit times + 50ms).
    //
    // Nowadays the usage of USB to serial adapter is very common, which could
    // result in additional delay of 100 ms in worst case.
    //
    // For 2400Bd this means (330 + 11) / 2400 + 0.15 = 292 ms (added 11 bit periods to receive first byte).
    // I.e. timeout of 0.3s seems appropriate for 2400Bd.
    switch (baudrate)
    {
        case 300:
            speed = B300;
            serial_data->t.c_cc[VTIME] = (cc_t) 13; // Timeout in 1/10 sec
            break;

        case 600:
            speed = B600;
            serial_data->t.c_cc[VTIME] = (cc_t) 8;  // Timeout in 1/10 sec
            break;

        case 1200:
            speed = B1200;
            serial_data->t.c_cc[VTIME] = (cc_t) 5;  // Timeout in 1/10 sec
            break;

        case 2400:
            speed = B2400;
            serial_data->t.c_cc[VTIME] = (cc_t) 3;  // Timeout in 1/10 sec
            break;

        case 4800:
            speed = B4800;
            serial_data->t.c_cc[VTIME] = (cc_t) 3;  // Timeout in 1/10 sec
            break;

        case 9600:
            speed = B9600;
            serial_data->t.c_cc[VTIME] = (cc_t) 2;  // Timeout in 1/10 sec
            break;

        case 19200:
            speed = B19200;
            serial_data->t.c_cc[VTIME] = (cc_t) 2;  // Timeout in 1/10 sec
            break;

        case 38400:
            speed = B38400;
            serial_data->t.c_cc[VTIME] = (cc_t) 2;  // Timeout in 1/10 sec
            break;

       default:
            return -1; // unsupported baudrate
    }

    // Set input baud rate
    if (cfsetispeed(&(serial_data->t), speed) == -1)
    {
        mbus_error_str_set("M-Bus serial failed setting input baudrate %d, error %d.",
			   speed, errno);
        return -1;
    }

    // Set output baud rate
    if (cfsetospeed(&(serial_data->t), speed) == -1)
    {
        mbus_error_str_set("M-Bus serial failed setting output baudrate %d, error %d.",
			   speed, errno);
        return -1;
    }

#ifdef MBUS_SERIAL_DEBUG
    printf("%s: t.c_cflag = %x\n", __func__, term->c_cflag);
    printf("%s: t.c_oflag = %x\n", __func__, term->c_oflag);
    printf("%s: t.c_iflag = %x\n", __func__, term->c_iflag);
    printf("%s: t.c_lflag = %x\n", __func__, term->c_lflag);
#endif

    // Change baud rate immediately
    if (tcsetattr(handle->fd, TCSANOW, &(serial_data->t)) == -1)
    {
        mbus_error_str_set("M-Bus serial failed tcsetattr(), error %d.", errno);
        return -1;
    }

    return 0;
}


//------------------------------------------------------------------------------
// Set parity for serial connection.  Not needed in common case, standard says
// 2400 8E1.  However, in some interop cases or for testing, some usb<->serial
// converters may not support parity bit (at all!).  This function is for such
// cases.  The mbus_serial_connect() function honors the standard.
//
// parity: 0 - off, 1 - odd, 2 - even (default: odd according to M-Bus std)
//------------------------------------------------------------------------------
int
mbus_serial_parity(mbus_handle *handle, int parity)
{
    mbus_serial_data *data;
    struct termios *c;

    if (handle == NULL)
    {
        mbus_error_str_set("M-Bus serial invalid parameter.");
	return -1;
    }

    data = (mbus_serial_data *)handle->auxdata;
    if (data == NULL)
    {
        mbus_error_str_set("M-Bus serial handle missing tty data ptr.");
        return -1;
    }

    c = &(data->t);
    switch (parity) {
    case 0:
	c->c_cflag &= ~PARENB;
	c->c_cflag &= ~PARODD;
	break;
    case 1:
	c->c_cflag |=  PARENB;
	c->c_cflag &= ~PARODD;
	break;
    case 2:
	c->c_cflag &= ~PARENB;
	c->c_cflag |=  PARODD;
	break;
    default:
	return -1;
    }

    return tcsetattr(handle->fd, TCSANOW, c);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int
mbus_serial_disconnect(mbus_handle *handle)
{
    if (handle == NULL)
    {
        mbus_error_str_set("M-Bus serial invalid parameter.");
        return -1;
    }

    if (handle->fd < 0)
    {
        /* Already closed. */
        return -1;
    }

    close(handle->fd);
    handle->fd = -1;

    return 0;
}

void
mbus_serial_data_free(mbus_handle *handle)
{
    mbus_serial_data *serial_data;

    if (handle)
    {
        serial_data = (mbus_serial_data *) handle->auxdata;

        if (serial_data == NULL)
        {
	    /* Already freed */
            return;
        }

        free(serial_data->device);
        free(serial_data);
        handle->auxdata = NULL;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int
mbus_serial_send_frame(mbus_handle *handle, mbus_frame *frame)
{
    unsigned char buff[PACKET_BUFF_SIZE];
    int len, ret;

    if (handle == NULL || frame == NULL)
    {
        mbus_error_str_set("M-Bus serial invalid parameter.");
        return -1;
    }

    // Make sure serial connection is open
    if (isatty(handle->fd) == 0)
    {
        mbus_error_str_set("M-Bus serial connection is not available.");
        return -1;
    }

    len = mbus_frame_pack(frame, buff, sizeof(buff));
    if (len < 0)
    {
	mbus_error_str_set("M-Bus serial failed mbus_frame_pack(), ret %d", len);
        return -1;
    }

#ifdef MBUS_SERIAL_DEBUG
    // if debug, dump in HEX form to stdout what we write to the serial port
    printf("%s: Dumping M-Bus frame [%d bytes]: ", __func__, len);
    int i;
    for (i = 0; i < len; i++)
    {
       printf("%.2X ", buff[i]);
    }
    printf("\n");
#endif

    ret = write(handle->fd, buff, len);
    if (ret == -1)
    {
        mbus_error_str_set("M-Bus serial failed writing frame to socket, error %d", errno);
        return -1;
    }

    //
    // call the send event function, if the callback function is registered
    //
    if (handle->send_event)
        handle->send_event(MBUS_HANDLE_TYPE_SERIAL, (char *)buff, len);

	//
    // wait until complete frame has been transmitted
    //
    tcdrain(handle->fd);

    return 0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int
mbus_serial_recv_frame(mbus_handle *handle, mbus_frame *frame)
{
    char buff[PACKET_BUFF_SIZE];
    int remaining, timeouts;
    ssize_t len, nread;

    if (handle == NULL || frame == NULL)
    {
        mbus_error_str_set("M-Bus serial invalid parameter.");
        return MBUS_RECV_RESULT_ERROR;
    }

    // Make sure serial connection is open
    if (isatty(handle->fd) == 0)
    {
        mbus_error_str_set("M-Bus serial connection is not available.");
        return MBUS_RECV_RESULT_ERROR;
    }

    memset((void *)buff, 0, sizeof(buff));

    //
    // read data until a packet is received
    //
    remaining = 1; // start by reading 1 byte
    len = 0;
    timeouts = 0;

    do {
        if (len + remaining > PACKET_BUFF_SIZE)
        {
            // avoid out of bounds access
	    mbus_error_str_set("M-Bus serial aborting, too small receive buffer!");
            return MBUS_RECV_RESULT_ERROR;
        }

#ifdef MBUS_SERIAL_DEBUG
        printf("%s: Attempt to read %d bytes [len = %d]\n", __func__, remaining, len);
#endif

        if ((nread = read(handle->fd, &buff[len], remaining)) == -1)
        {
            mbus_error_str_set("M-Bus serial aborting receive frame, errno %d\n", errno);
            return MBUS_RECV_RESULT_ERROR;
        }

#ifdef MBUS_SERIAL_DEBUG
        printf("%s: Got %d byte [remaining %d, len %d]\n", __func__, nread, remaining, len);
#endif

        if (nread == 0)
        {
            timeouts++;

            if (timeouts >= 3)
            {
                // abort to avoid endless loop
                break;
            }
        }

        if (len > (SSIZE_MAX-nread))
        {
            // avoid overflow
	    mbus_error_str_set("M-Bus serial aborting, would overflow buffer!");
            return MBUS_RECV_RESULT_ERROR;
        }

        len += nread;

    } while ((remaining = mbus_parse(frame, (unsigned char *)buff, len)) > 0);

    if (len == 0)
    {
        // No data received
	mbus_error_str_set("M-Bus serial receive timeout!");
        return MBUS_RECV_RESULT_TIMEOUT;
    }

    //
    // call the receive event function, if the callback function is registered
    //
    if (handle->recv_event)
        handle->recv_event(MBUS_HANDLE_TYPE_SERIAL, buff, len);

    if (remaining != 0)
    {
        // Would be OK when e.g. scanning the bus, otherwise it is a failure.
	mbus_error_str_set("M-Bus serial failed to receive complete data.");
        return MBUS_RECV_RESULT_INVALID;
    }

    if (len == -1)
    {
	mbus_error_str_set("M-Bus serial receive failed to parse data.");
        return MBUS_RECV_RESULT_ERROR;
    }

    return MBUS_RECV_RESULT_OK;
}
