/*
 * consts.h
 *
 *  Created on: 26 Sep 2012
 *      Author: jack
 */

#ifndef CONSTS_H_
#define CONSTS_H_

#include <inttypes.h>

typedef uint32_t id_t;     // type for storing IDs
typedef uint32_t millis_t; // type for storing times in milliseconds
typedef uint16_t watts_t;  // type for storing watts
typedef uint8_t  index_t;  // type for storing indices

#define INDEX_MAX 255

enum TxType {TRX, TX};

// Values denoting invalid status
const watts_t WATTS_INVALID   = 0xFFFF;
const id_t    ID_INVALID      = 0xFFFFFFFF;
const index_t  INVALID_INDEX  = 0xFF;

// Consts defining behaviour of system
const millis_t SAMPLE_PERIOD  = 6000; /* miliseconds    */




#endif /* CONSTS_H_ */
