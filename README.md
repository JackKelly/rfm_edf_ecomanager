
Uploading pre-compiled hex file
===============================

First change directory into the Release/ directory.

Uploading to AVR using FTDI serial cable:
-----------------------------------------

```bash
avrdude -pm328p -carduino -P/dev/ttyUSB0 -b57600 -F -V 
            -Uflash:w:rfm_edf_ecomanager.hex:a
```

Uploading to AVR using STK500v2 programmer:
-------------------------------------------

```bash
avrdude -pm328p -cstk500v2 -P/dev/ttyACM0 -Uflash:w:rfm_edf_ecomanager.hex:a
```

Building using Eclipse
======================

I'm afraid building this code isn't especially user-friendly at the moment
because I'm developing on Eclipse and am using the auto-generated makefile
at the moment while there's a lot of code churn. If I have time I'll manually
create a more user friendly makefile.

The easiest way to start development on this code right now is probably to 
setup Eclipse using the notes at jack-kelly.com/eclipse_notes
and import this repository's .cpp and .h files as
an AVR C++ project (don't import the stuff in this repository's Release/
directory: Eclipse should make that for you automatically.)

Building without Eclipse
========================

I haven't tried this so I'm afraid I don't know if this will work but
here are some pointers.

First, open Release/subdir.mk.  You'll almost certainly need to modify
the include directories to point to the relevant directories on 
your system.

Then run:

```bash
cd Release
make all
```
Then upload your rfm_edf_ecomanager.hex using the "Uploading pre-compiled
hex file" instructions above.
