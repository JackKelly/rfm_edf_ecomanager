/**
 * Main.
 *
 * ----------------------
 * Terminology and naming
 * ----------------------
 *
 * All EDF EcoManager and CurrentCost sensors can be split into two categories:
 *
 *  - cc_trx (short for "transceiver") sensors which can transmit and receive.
 *    e.g. EDF IAMs.
 *    TRX sensors don't transmit every six seconds, instead they sit patiently
 *    and wait to be polled.
 *
 *  - cc_tx (short for "transmitter") sensors which have no ability to receive.
 *    e.g. CurrentCost IAMs, CurrentCost Sensable transmitters, EDF Whole-House transmitters
 *
 */

#ifdef ARDUINO
#include <inttypes.h>
#else
#include <Arduino.h>
#endif

#include "Manager.h"
#include "Logger.h"

Manager manager;

void setup()
{
    Serial.begin(115200);

    log(INFO, "EDF EcoManager Receiver");
    manager.init();
    log(INFO, "Finished init");
}

void loop()
{
	manager.run();
}

int main(void)
{
  init();
  setup();

  while(true) {
    loop();
  }
}

