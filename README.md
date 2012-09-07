Code for communicating wirelessly with [Current Cost home energy monitors](http://www.currentcost.com/products.html)
and [EDF EcoManager](http://www.edfenergy.com/products-services/for-your-home/ecomanager/) monitors.  The main planned aim of this code is the
robust gathering of data from an arbitrary number of EDF EcoManager
[Wireless Transmitter Plugs](https://shop.edfenergy.com/Item.aspx?id=540&CategoryID=1) for tracking the state of the majority of appliances
in a building.

Current status
==============

At present the code is in a very early stage.  The code simply transmits a
hard-coded ping to my EDF IAM and displays all responses.  The code as-is
will capture and display all Current Cost or EDF EcoManager RF packets it
receives.  Note that the RX packets are buffered and the buffer is displayed
and emtpied once every 5 seconds.


Hardware requirements
=====================

 - The easiest way to get started is to buy a [Nanode Classic](http://www.nanode.eu/products/)
   and a 433MHz [RFM12b](http://www.hoperf.com/rf_fsk/fsk/21.htm) wireless transceiver.  Note that the Nanode RF kit
   comes with an 868 MHz RFM12b, which is not what you want.

 - If you build a DIY board, or if you connect an RFM12b to an Arduino,
   make sure the RFM12b's nFFS pin is set permanently high.


Usage
=====

 - Load this program onto your ATmega. The easiest way to do this is to use
   the pre-compiled hex file (see "[Uploading pre-compiled hex file](#uploading-pre-compiled-hex-file)" below).  

 - Then watch the serial port.  On Linux this can be achieved with
   the command: `screen /dev/ttyUSB0 115200`

 - You should see each RF packet displayed on your screen (each byte
   displayed as hex).


Acknowledgements & further reading
==================================

 - Many thanks to the good folks who wrote [JeeLib](https://github.com/jcw/jeelib) as it provided lots
   of help.

 - As did [this tutorial on SPI on AVR devices](https://sites.google.com/site/qeewiki/books/avr-guide/spi).

 - If you want an indepth insight into the RFM01 then read the [RFM12B.pdf](http://www.hoperf.com/upload/rf/RFM12B.pdf)
   manual BEFORE or INSTEAD OF [RFM12B_code.pdf](http://www.hoperf.com/upload/rf/RF12B_code.pdf) as the latter has far less detail.
   The only advantage of the RFM12B_code.pdf is that it contains a
   code example in C.

 - For more details please see [the CurrentCost category on my blog](http://jack-kelly.com/taxonomy/term/121).


Uploading pre-compiled hex file
===============================

First change directory into the `Release/` directory.

Uploading to AVR using FTDI serial cable:

```bash
avrdude -pm328p -carduino -P/dev/ttyUSB0 -b57600 -F -V 
            -Uflash:w:rfm_edf_ecomanager.hex:a
```

Uploading to AVR using STK500v2 programmer:

```bash
avrdude -pm328p -cstk500v2 -P/dev/ttyACM0 -Uflash:w:rfm_edf_ecomanager.hex:a
```

Building from source
====================

Using Eclipse
-------------

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
------------------------

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
Then upload your `rfm_edf_ecomanager.hex` using the "[Uploading pre-compiled hex file](#uploading-pre-compiled-hex-file)"
instructions above.

Using the Arduino IDE
---------------------

If you're using an Arduino IDE then you'll definitely need to delete the
`main()` function.  I'm afraid I don't use the Arduino IDE so I'm not
sure what else will be necessary to get this code to work in the Aduino IDE.
