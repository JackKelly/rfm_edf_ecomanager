#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
    enum Level {DEBUG, INFO, WARN, ERROR, FATAL};
    static Level log_threshold;

    static void log(const Level& level, const char *__fmt, ...);
    //static void print_level(const Level& level);
    //static void print_levels();
};

#endif // LOGGER_H
