#include <Arduino.h>
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

