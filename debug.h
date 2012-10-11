#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG

enum DebugLevel {INFO, WARN, ERROR, FATAL};

void debug(const DebugLevel debug_level, const char *__fmt, ...);

#endif // DEBUG_H
