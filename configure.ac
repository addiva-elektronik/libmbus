dnl ----------------------------------------------------------------------------
dnl Copyright (C) 2010, Raditex AB
dnl All rights reserved.
dnl
dnl rSCADA 
dnl http://www.rSCADA.se
dnl info@rscada.se
dnl 
dnl ----------------------------------------------------------------------------

LT_CONFIG_LTDL_DIR([libltdl])

# Remember to bump the ABI version in mbus/Makefile.am.  For details, see:
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
AC_INIT([libmbus], [0.9.0], [info@rscada.se], [libmbus], [http://www.rscada.se/libmbus/])
AC_CONFIG_AUX_DIR([libltdl/config])
AM_INIT_AUTOMAKE([-W -Wall foreign])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
	mbus/Makefile
	test/Makefile
	bin/Makefile
	libmbus.pc])

AM_PROG_AR
AC_PROG_CC
LT_INIT

AC_CHECK_HEADERS([termios.h])

AC_OUTPUT


echo \
"----------------------------------------------------------
Configuration:

	Source location:	${srcdir}
	Compile:		${CC}
	Compiler flags:		${CFLAGS}
	Linker flags:		${LDFLAGS}
	Host system type:	${host}
	Install path:		${prefix}

	See config.h for further configuration.
----------------------------------------------------------"
