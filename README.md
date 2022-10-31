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


Memory Usage
------------

The library is fairly conservative with RAM, below is a run using an
example [M-Bus master]() that talks to an example [M-Bus device]()
over a [virtual serial bus]().  Memory usage has been analyzed with
the valgrind tool massif.

An example run where the master sends a data request to the device
and prints it in verbose format (default: raw HEX dump), toggles to
XML output and does the same request.

> **Note:** below stack usage in this run peaks at 4416 bytes, which was
> determined to occur in the C runtime (crt0) well before the `main()`
> function is started.  In some runs the stack peaked in crt0 at 7736
> bytes (!) before falling back to reasonable levels in `main()`.

```
$ ./vsb -n 2 & sleep 1
vsb: device 1 => /dev/pts/8
vsb: device 2 => /dev/pts/9
$ ../mbus-device/mbus-device -a 5 /dev/pts/9 -f ~/src/libmbus/test/test-frames/abb_delta.hex &
$ valgrind --tool=massif --stacks=yes ./mbus-master /dev/pts/8 -v
...
> req 5
...
> xml
> req 5
...
$ ms_print massif.out.842178
--------------------------------------------------------------------------------
  n        time(i)         total(B)   useful-heap(B) extra-heap(B)    stacks(B)
--------------------------------------------------------------------------------
  0              0                0                0             0            0
  1          2,598              512                0             0          512
  2         40,725              512                0             0          512
  3         51,678            1,520                0             0        1,520
  4         75,611            4,416                0             0        4,416
  5         92,251            1,376                0             0        1,376
... before main() has started, inside crt0.o somewhere, seen up to 7,736 stack!
 37        720,368           21,016           17,019           301        3,696
 38        737,529           25,496           23,006           346        2,144
 39        755,851           22,672           18,520           400        3,752
...
 52        960,449           21,552           18,520           400        2,632
...
 63        989,236            5,776            3,728           152        1,896
 64        991,834            4,136            3,549           115          472
 65        994,435              504                0             0          504
--------------------------------------------------------------------------------
```


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
