#!/usr/bin/env bash

/usr/bin/avrdude -pm328p -cstk500v2 -P/dev/ttyACM0 -F -Uflash:w:Release/rfm_edf_ecomanager.hex:a # -C/usr/share/arduino/hardware/tools/avrdude.conf 

