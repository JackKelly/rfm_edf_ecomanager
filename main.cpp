#include <Arduino.h>
#include "Rfm12b.h"

Rfm12b RFM12B;

void setup()
{
    Serial.begin(115200);
    Serial.println("EDF EcoManager Receiver");
    RFM12B.init_edf();
    RFM12B.enable_rx();
    Serial.println("Finished init");
}

void loop()
{
	delay(4900);
	Serial.println(".");
	RFM12B.ping_edf_iam();
	delay(100);
	RFM12B.print_if_data_available();
}

int main(void)
{
  init();
  setup();

  while(true) {
    loop();
  }
}

