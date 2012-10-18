#ifndef LOGGER_H
#define LOGGER_H

#ifndef TESTING

enum Level {DEBUG, INFO, WARN, ERROR, FATAL};
void print_log_level(const Level& level);
void print_log_levels();
void log(const Level& level, const char *__fmt, ...);

class Logger {
public:
    static Level log_threshold;
};

#endif // TESTING

#endif // LOGGER_H
