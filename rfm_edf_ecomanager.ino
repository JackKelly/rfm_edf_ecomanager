/* These includes are necessary to force the Arduino IDE to compile
 * and link all the necessary files.
 * This technique was taken from http://provideyourown.com/2011/advanced-arduino-including-multiple-libraries/ */
#include "spi.h"
#include "Packet.h"
#include "Rfm12b.h"
#include "Manager.h"

/* Include the actual main function for the code: */
#include "rfm_edf_ecomanager.cpp"
