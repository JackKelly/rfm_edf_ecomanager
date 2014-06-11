This code runs on a [Nanode](http://www.nanode.eu/) / [Arduino](http://www.arduino.cc/) with an HopeRF RFM12b 433MHz wireless module and wirelessly communicates with [Current Cost home energy monitors](http://www.currentcost.com/products.html) and [EDF EcoManager](http://www.edfenergy.com/products-services/for-your-home/ecomanager/) monitors.  The main aim of this code is the robust gathering of data from an arbitrary number of EDF EcoManager
[Wireless Transmitter Plugs](https://shop.edfenergy.com/Item.aspx?id=540&CategoryID=1) and a small number of
Current Cost whole-house "sensable" transmitters for tracking the state of the majority of appliances
in a building.

Related projects
================

My [RFM_ecomanager_logger](https://github.com/JackKelly/rfm_ecomanager_logger) code logs data from rfm_edf_ecomanager on a PC.


Current status
==============

The code is "feature complete" in that it implements all of the features listed in the
[user manual](https://github.com/JackKelly/rfm_edf_ecomanager/wiki).


Known issues
============

The code doesn't work especially well with Current Cost Individual Appliance Monitors (CC IAMs).  See [this issue](https://github.com/JackKelly/rfm_edf_ecomanager/issues/13) for more details.


More info
=========

Please see the [user manual](https://github.com/JackKelly/rfm_edf_ecomanager/wiki) for more info.
