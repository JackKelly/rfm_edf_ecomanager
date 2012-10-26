/*
 * FakeArduino.h
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#ifndef FAKEARDUINO_H_
#define FAKEARDUINO_H_

#include <inttypes.h>
#include "../consts.h"

typedef uint8_t byte;

#define PSTR(X) X
#define F(X) X

enum PrintFormat {DEC, HEX};

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
    static void print(const char* str, const PrintFormat print_format = DEC);
    static void print(const int& str, const PrintFormat print_format = DEC);
    static void print(const unsigned int& str, const PrintFormat print_format = DEC);

    static void println(const char* str, const PrintFormat print_format = DEC);
private:
    static void format(const PrintFormat print_format);
};

extern FakeSerial Serial;

const millis_t millis();

#endif /* FAKEARDUINO_H_ */
