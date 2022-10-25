# ------------------------------------------------------------------------------
# Copyright (C) 2010, Raditex AB
# All rights reserved.
#
# rSCADA 
# http://www.rSCADA.se
# info@rscada.se
#
# ------------------------------------------------------------------------------
includedir         = $(prefix)/include/mbus
include_HEADERS    = mbus.h mbus-protocol.h mbus-tcp.h mbus-serial.h mbus-protocol-aux.h

# Rules for how (and when!) to update the ABI version, see:
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
lib_LTLIBRARIES	   = libmbus.la
libmbus_la_SOURCES = mbus.c mbus-protocol.c mbus-tcp.c mbus-serial.c mbus-protocol-aux.c
libmbus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined -version-info 0:9:0

