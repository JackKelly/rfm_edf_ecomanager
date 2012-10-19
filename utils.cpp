#include <Arduino.h>
#include "consts.h"
#include "utils.h"

void utils::read_cstring_from_serial(char* str, const index_t& length)
{
    index_t i = 0;
    str[0] = '\0';

    do {
        if (Serial.available()) {
            str[i++] = Serial.read();
            Serial.print(str[i-1]); // echo
            if (i == length-1) break;
        }
    } while (str[i-1] != '\r');

    Serial.println("");
    Serial.flush();
    str[i] = '\0';
}

const uint32_t utils::read_uint32_from_serial()
{
    const index_t BUFF_LENGTH = 15; /* 10 chars + 1 sentinel char + extras for whitespace.
                                     * The max value a uint32 can store is
                                     * 4 billion (10 chars decimal). */
    char buff[BUFF_LENGTH];
    read_cstring_from_serial(buff, BUFF_LENGTH);
    return strtoul(buff, NULL, 0);
}
