Outstanding Issues
==================

Things to look into, refactor, or clean up.


 - [ ] Add (or refactor existing) functions for scanning primary+secondary addresses to provide callbacks when finding devices.  Currently the results are printed to `stdout` continuously during the scanning operation.  Make sure to update example code in `bin/` accordingly.
   - [X] Secondary scanning fixed in fc058d4
 - [ ] Refactor secondary scanning function, or provide an alternative, to not call itself recursively (measure stack use in worst case first!)
 - [ ] Go through upstream's outstanding issues and pull requests, may be relevant bug fixes and/or features (already hand-merged Ingo Fisher's GitHub actions).
 - [ ] `mbus_data_variable_xml_normalized()` ... I have no words, maybe `#ifdef` all these par functions behind a configure option?  I.e., `--enable-really-small` or sth
 - ...
