/*
 * consts.h
 *
 *  Created on: 26 Sep 2012
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

#ifndef CONSTS_H_
#define CONSTS_H_

#include <inttypes.h>

typedef uint32_t id_t;     /* type for storing IDs */
typedef uint32_t millis_t; /* type for storing times in milliseconds */
typedef uint16_t watts_t;  /* type for storing watts */
typedef uint8_t  index_t;  /* type for storing indices */

#define INDEX_MAX 255

enum TxType {TRX, TX};

/* Values denoting invalid status */
const watts_t WATTS_INVALID   = 0xFFFF;
const id_t    ID_INVALID      = 0xFFFFFFFF;
const index_t  INVALID_INDEX  = 0xFF;

/* Consts defining behaviour of system */
const millis_t SAMPLE_PERIOD  = 6000; /* miliseconds    */

#endif /* CONSTS_H_ */
