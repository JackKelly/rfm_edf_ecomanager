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
    case DEBUG: Serial.print("(DEBUG)"); break;
    case INFO: Serial.print("(INFO)"); break;
    case WARN: Serial.print("(WARN)"); break;
    case ERROR: Serial.print("(ERROR)"); break;
    case FATAL: Serial.print("(FATAL)"); break;
    }
#endif // LOGGING
}

inline
void print_log_levels()
{
#ifdef LOGGING
    for (uint8_t level=DEBUG; level<FATAL; level+=1) {
        print_log_level((Level)level);
        Serial.print(", ");
    }
    print_log_level(FATAL);
    Serial.println("");
#endif // LOGGING
}


inline void log(const Level& level, const char *__fmt, ...)
{
#ifdef LOGGING
    if (Logger::log_threshold <= level) {
        Serial.print(millis());
        Serial.print(" ");
        print_log_level(level);
        Serial.print(" ");

        char buf[64];

        va_list vl;
        va_start(vl, __fmt);
        vsprintf(buf, __fmt, vl);
        va_end(vl);

        Serial.println(buf);
    }
#endif // LOGGING
}


#endif // TESTING

#endif // LOGGER_H
