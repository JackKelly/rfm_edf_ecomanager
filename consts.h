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

#ifdef TESTING
#include <inttypes.h>
#else
#include <Arduino.h>
#endif

typedef uint32_t id_t;     /* type for storing IDs */
typedef uint32_t millis_t; /* type for storing times in milliseconds */
typedef uint16_t watts_t;  /* type for storing watts */
typedef uint8_t  index_t;  /* type for storing indices */

#define INDEX_MAX 255

enum TxType {TRX, TX};

/* Values denoting invalid status */
const watts_t WATTS_INVALID   = 0xFFFF;
const id_t    ID_INVALID      = 0xFFFFFFFF;
const index_t INVALID_INDEX   = 0xFF;
const uint32_t UINT32_INVALID = 0xFFFFFFFF;

/* Consts defining behaviour of system */
const millis_t SAMPLE_PERIOD  = 6000; /* milliseconds    */

/* length of time we're willing to wait
 * for a CC TX.  We'll open the window
 * half of WINDOW before the next CC TX's ETA. */
const uint16_t CC_TX_WINDOW = 500;
const uint16_t CC_TX_WINDOW_OPEN = CC_TX_WINDOW / 2;

const uint8_t CC_TRX_TIMEOUT = 90; /* milliseconds to wait for reply from TRX */
const uint8_t MAX_RETRIES = 5; /* Max num times we'll try to poll a TRX per roll call */

const index_t PACKET_BUF_LENGTH = 5; /* Number of packets in RX packet buffer */

const millis_t KEYPRESS_TIMEOUT = 5000; /* milliseconds */

#endif /* CONSTS_H_ */
