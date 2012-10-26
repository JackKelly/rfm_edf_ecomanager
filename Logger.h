/*
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

#ifndef LOGGER_H
#define LOGGER_H

#ifndef TESTING

#include <Arduino.h>

enum Level {DEBUG, INFO, WARN, ERROR, FATAL};

#ifdef LOGGING
class Logger {
public:
    static Level log_threshold;
};
#endif // LOGGING

inline
void print_log_level(const Level& level)
{
#ifdef LOGGING
    Serial.print(level);

    switch (level) {
    case DEBUG: Serial.print(F("(DEBUG)")); break;
    case INFO: Serial.print(F("(INFO)")); break;
    case WARN: Serial.print(F("(WARN)")); break;
    case ERROR: Serial.print(F("(ERROR)")); break;
    case FATAL: Serial.print(F("(FATAL)")); break;
    }
#endif // LOGGING
}

inline
void print_log_levels()
{
#ifdef LOGGING
    for (uint8_t level=DEBUG; level<FATAL; level+=1) {
        print_log_level((Level)level);
        Serial.print(F(", "));
    }
    print_log_level(FATAL);
    Serial.println(F(""));
#endif // LOGGING
}

#ifdef LOGGING
inline
void flash_strcpy(char* dst, const char* src)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"

    while (*dst++ = pgm_read_byte(src++))
        ;

#pragma GCC diagnostic pop
}
#endif

inline void log(const Level& level, const char *__fmt, ...)
{
#ifdef LOGGING
    const uint8_t LENGTH = 64;
    if (Logger::log_threshold <= level) {
        Serial.print(millis());
        Serial.print(F(" "));
        print_log_level(level);
        Serial.print(F(" "));

        char fmt[LENGTH];
        flash_strcpy(fmt, __fmt);

        char buf[LENGTH];

        va_list vl;
        va_start(vl, __fmt);
        vsprintf(buf, fmt, vl);
        va_end(vl);

        Serial.println(buf);
    }
#endif // LOGGING
}


#endif // TESTING

#endif // LOGGER_H
