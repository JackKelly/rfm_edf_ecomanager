/**
 * Main
 */

#ifdef ARDUINO
#include <inttypes.h>
#else
#include <Arduino.h>
#endif

#include "Manager.h"

Manager manager;

void setup()
{
    Serial.begin(115200);
    Serial.println("EDF EcoManager Receiver");
    manager.init();
    Serial.println("Finished init");
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

// TODO: write code to respond to requests from serial:
//      - pair with IAM currently requesting to be paired
//           - add this to list on Nanode and send UID over serial
//      - send over serial a list of all IAMs paired
