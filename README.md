Open Source M-bus (Meter-Bus) Library
-------------------------------------
[![License Badge][]][License] [![GitHub Status][]][GitHub]

libmbus is an open source library for the M-bus (Meter-Bus) protocol.

The Meter-Bus is a standard for reading out meter data from electricity
meters, heat meters, gas meters, etc.  The M-bus standard deals with
both the electrical signals on the M-Bus, and the protocol and data
format used in transmissions on the M-Bus.  The role of libmbus is to
decode/encode M-bus data, and to handle the communication with M-Bus
devices.

Wired M-Bus Specification (v4.8):

 - <https://m-bus.com/documentation-wired/01-introduction>
 - <https://m-bus.com/assets/downloads/MBDOC48.PDF>

Manufacturer ID list:

 - <https://www.m-bus.de/man.html>

Example M-Bus Master:

 - <https://github.com/addiva-elektronik/mbus-master>

Example M-Bus Device (slave):

 - <https://github.com/addiva-elektronik/mbus-device>


Building
--------

    ./autogen.sh             # only needed from git clone
    ./configure
    make

> **Note:** libmbus relies on the math function `pow()` so when you link
> it with your own program ensure you have `-lm`.  When using
> `pkg-config` to query the system for appropriate `CFLAGS` and `LIBS`,
> this is handled automatically for you.  However, when building on an
> embedded system you may not have `pow()`, to this end a replacement
> is available in `lib/pow.c`.


Origin & References
-------------------

This is a fork of the upstream https://github.com/rscada/libmbus project
with a focus on porting to microcontrollers running real-time operating
systems like [Zephyr](https://www.zephyrproject.org/).

libmbus is originally from [Raditex Control](http://www.rscada.se), see their
original page at <http://www.rscada.se/libmbus>.

[License]:          https://en.wikipedia.org/wiki/BSD_licenses
[License Badge]:    https://img.shields.io/badge/License-BSD%203--Clause-blue.svg
[GitHub]:           https://github.com/addiva-elektronik/libmbus/actions/workflows/build.yml/
[GitHub Status]:    https://github.com/addiva-elektronik/libmbus/actions/workflows/build.yml/badge.svg
