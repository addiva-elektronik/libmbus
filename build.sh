#!/bin/sh
#

if [ -f Makefile ]; then
    # use existing automake files
    echo >> /dev/null
else
    # regenerate automake files
    echo "Running autotools..."
    ./autogen.sh && ./configure
fi

make
