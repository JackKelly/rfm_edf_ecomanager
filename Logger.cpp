#include <Arduino.h>
#include "Logger.h"

Level Logger::log_threshold = INFO;

void log(const Level& level, const char *__fmt, ...)
{
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
}



void print_log_level(const Level& level)
{
    Serial.print(level);

    switch (level) {
    case DEBUG: Serial.print("(DEBUG)"); break;
    case INFO: Serial.print("(INFO)"); break;
    case WARN: Serial.print("(WARN)"); break;
    case ERROR: Serial.print("(ERROR)"); break;
    case FATAL: Serial.print("(FATAL)"); break;
    }

}


void print_log_levels()
{
    for (uint8_t level=DEBUG; level<FATAL; level+=1) {
        print_log_level((Level)level);
        Serial.print(", ");
    }
    print_log_level(FATAL);
    Serial.println("");
}
