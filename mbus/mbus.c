//------------------------------------------------------------------------------
// Copyright (C) 2010, Raditex AB
// All rights reserved.
//
// rSCADA
// http://www.rSCADA.se
// info@rscada.se
//
//------------------------------------------------------------------------------

#include "mbus-protocol.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//
//
//
int mbus_init() {return 0;}

///
/// Return current version of the library
///
const char*
mbus_get_current_version() {return VERSION;}
