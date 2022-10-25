# ------------------------------------------------------------------------------
# Copyright (C) 2010, Raditex AB
# All rights reserved.
#
# rSCADA 
# http://www.rSCADA.se
# info@rscada.se
#
# ------------------------------------------------------------------------------  
AUTOMAKE_OPTIONS        = subdir-objects
noinst_PROGRAMS         = mbus_parse mbus_parse_hex

mbus_parse_SOURCES      = mbus_parse.c
mbus_parse_LDFLAGS      = -L$(top_builddir)/mbus
mbus_parse_LDADD        = -lmbus $(LIBS) $(LIBOBJS)

mbus_parse_hex_SOURCES  = mbus_parse_hex.c
mbus_parse_hex_LDFLAGS  = -L$(top_builddir)/mbus
mbus_parse_hex_LDADD    = -lmbus $(LIBS) $(LIBOBJS)
