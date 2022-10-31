Outstanding Issues
==================

Things to look into, refactor, or clean up.


 - [ ] Add (or refactor existing) functions for scanning primary+secondary addresses to provide callbacks when finding devices.  Currently the results are printed to `stdout` continuously during the scanning operation.  Make sure to update example code in `bin/` accordingly.
   - [X] Secondary scanning fixed in fc058d4
 - [ ] Refactor secondary scanning function, or provide an alternative, to not call itself recursively (measure stack use in worst case first!)
 - [ ] Go through upstream's outstanding issues and pull requests, may be relevant bug fixes and/or features (already hand-merged Ingo Fisher's GitHub actions).
 - [ ] Audit record parser, seems to be wrong when testing with std. pp 43, variable data structure (mode 1), which is the default built-in data in [mbus-device][].  See more below.
 - [ ] `mbus_data_variable_xml_normalized()` ... I have no words, maybe `#ifdef` all these par functions behind a configure option?  I.e., `--enable-really-small` or sth
 - ...



Example run with mbus-device
----------------------------

The [mbus-device][] implements a built-in device simulating a variable
data structure (mode 1) from std. pp 43:

```
68 1F 1F 68            header of RSP_UD telegram (length 1Fh=31d bytes)
08 02 72               C field = 08 (RSP), address 2, CI field 72H (var.,LSByte first)
78 56 34 12            identification number = 12345678
24 40 01 07            manufacturer ID = 4024h (PAD in EN 61107), generation 1, water
55 00 00 00            TC = 55h = 85d, Status = 00h, Signature = 0000h
03 13 15 31 00         Data block 1: unit 0, storage No 0, no tariff, instantaneous volume, 12565 l (24 bit integer)
DA 02 3B 13 01         Data block 2: unit 0, storage No 5, no tariff, maximum volume flow, 113 l/h (4 digit BCD)
8B 60 04 37 18 02      Data block 3: unit 1, storage No 0, tariff 2, instantaneous energy, 218,37 kWh (6 digit BCD)
18 16                  checksum and stopsign
```

With libmbus this currently gives us the following XML output.  Notice
the invalid data unit and incomplete conversion of energy.

```
<?xml version="1.0" encoding="ISO-8859-1"?> <MBusData>

    <SlaveInformation>
        <Id>12345678</Id>
        <Manufacturer>PAD</Manufacturer>
        <Version>1</Version>
        <ProductName></ProductName>
        <Medium>Water</Medium>
        <AccessNumber>85</AccessNumber>
        <Status>00</Status>
        <Signature>0000</Signature>
    </SlaveInformation>

    <DataRecord id="0">
        <Function>Instantaneous value</Function>
        <StorageNumber>0</StorageNumber>
        <Unit>Volume (m m^3)</Unit>
        <Value>12565</Value>
        <Timestamp>2022-09-31T11:14:41Z</Timestamp>
    </DataRecord>

    <DataRecord id="1">
        <Function>Maximum value</Function>
        <StorageNumber>5</StorageNumber>
        <Tariff>0</Tariff>
        <Device>0</Device>
        <Unit>Volume flow (m m^3/h)</Unit>
        <Value>113</Value>
        <Timestamp>2022-09-31T11:14:41Z</Timestamp>
    </DataRecord>

    <DataRecord id="2">
        <Function>Instantaneous value</Function>
        <StorageNumber>0</StorageNumber>
        <Tariff>2</Tariff>
        <Device>1</Device>
        <Unit>Energy (10 Wh)</Unit>
        <Value>21837</Value>
        <Timestamp>2022-09-31T11:14:41Z</Timestamp>
    </DataRecord>

</MBusData>
```


[mbus-device]: https://github.com/addiva-elektronik/mbus-device
