#include <Arduino.h>
#include "utils.h"

void utils::read_cstring_from_serial(char* str, const uint8_t& length)
{
    uint8_t i = 0;
    str[0] = '\0';

    do {
        if (Serial.available()) {
            str[i++] = Serial.read();
            if (i == length-1) break;
        }
    } while (str[i-1] != '\r');

    str[i] = '\0';
}

const uint32_t utils::read_uint32_from_serial()
{
    const uint8_t BUFF_LENGTH = 15; /* 10 chars + 1 sentinel char + extras for whitespace.
                                     * The max value a uint32 can store is
                                     * 4 billion (10 chars decimal). */
    char buff[BUFF_LENGTH];
    read_cstring_from_serial(buff, BUFF_LENGTH);
    return strtoul(buff, NULL, 0);
}
