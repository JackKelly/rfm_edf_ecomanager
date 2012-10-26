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
void read_cstring_from_serial(char* str, const index_t& length);

const uint32_t read_uint32_from_serial();

/**
 * @returns true if deadline is in the future.
 * Should handle roll-over of millis() correctly.
 */
const bool in_future(const millis_t& deadline);

};

#endif // CC_UTILS_H
