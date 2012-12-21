/**
 * Main.
 *
 *      Author: Jack Kelly
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE
 * LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
 * PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE
 * DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */

#include "Manager.h"
#include "Logger.h"

Manager manager;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("EDF IAM Receiver"));
    manager.init();
    Serial.println(F("Finished init"));
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
