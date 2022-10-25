Open Source M-bus Library
-------------------------
[![License Badge][]][License] [![GitHub Status][]][GitHub]

libmbus is an open source library for the M-bus (Meter-Bus) protocol.

The Meter-Bus is a standard for reading out meter data from electricity meters,
heat meters, gas meters, etc. The M-bus standard deals with both the electrical
signals on the M-Bus, and the protocol and data format used in transmissions on
the M-Bus. The role of libmbus is to decode/encode M-bus data, and to handle
the communication with M-Bus devices.


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
