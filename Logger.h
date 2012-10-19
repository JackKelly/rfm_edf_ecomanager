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
#endif // DEBUG
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
#endif // DEBUG
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
