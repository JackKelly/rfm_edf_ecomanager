#ifndef UTILS_H
#define UTILS_H

namespace utils {

/**
 * Reads C string from serial port.
 * Ends if length-1 chars are read or if carriage return is received.
 * Always adds sentinel char (so make sure str[] is long enough
 * for your string plus sentinel char).
 * Blocks until '\r' (carriage return) is received.
 */
void read_cstring_from_serial(char* str, const uint8_t& length);

const uint32_t read_uint32_from_serial();

};

#endif // CC_UTILS_H
