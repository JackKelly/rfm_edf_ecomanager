/*
 * FakeArduino.h
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#ifndef FAKEARDUINO_H_
#define FAKEARDUINO_H_

#include <inttypes.h>

enum Level {DEBUG, INFO, WARN, ERROR, FATAL};
void print_log_level(const Level& level);
void print_log_levels();
void log(const Level& level, const char *__fmt, ...);

class Logger {
public:
    static Level log_threshold;
};


class FakeSerial {
public:
    static void print(const char* str);
    static void print(const int str);

    static void println(const char* str);
};

extern FakeSerial Serial;

#endif /* FAKEARDUINO_H_ */
