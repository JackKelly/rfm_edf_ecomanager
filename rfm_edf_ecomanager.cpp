/**
 * Main.
 *
 */

#include <Arduino.h>
#include "new.h"
#include "Manager.h"
#include "Logger.h"

Manager manager;

void setup()
{
    Serial.begin(115200);

    Serial.println("EDF IAM Receiver");
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

