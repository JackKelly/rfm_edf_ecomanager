#include <Arduino.h>
#include "debug.h"

void debug(const DebugLevel debug_level, const char *__fmt, ...)
{
#ifdef DEBUG

    char buf[64];

    sprintf(buf, "%lu", millis());
    Serial.print(buf);

    switch(debug_level) {
    case INFO : Serial.print(" INFO: "); break;
    case WARN : Serial.print(" WARNING: "); break;
    case ERROR: Serial.print(" ERROR: "); break;
    case FATAL: Serial.print(" FATAL: "); break;
    }

    va_list vl;
    va_start(vl, __fmt);
    vsprintf(buf, __fmt, vl);
    va_end(vl);

    Serial.println(buf);
    
#endif // DEBUG
}
