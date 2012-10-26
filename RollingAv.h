/*
 * RollingAv.h
 *
 *  Created on: 19 Oct 2012
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

#ifndef ROLLINGAV_H_
#define ROLLINGAV_H_

#ifdef TESTING
#include <inttypes.h>
#else
#include <Arduino.h>
#endif

#include "consts.h"

/**
 * Class for keeping a rolling average
 */
class RollingAv {
public:
    RollingAv();
    void add_sample(const uint16_t& sample);
    const uint16_t& get_av();

private:
    static const index_t NUM_SAMPLES = 5;
    uint16_t samples[NUM_SAMPLES];
    index_t index;
    uint16_t av_cache;
    bool cache_valid;
};

#endif /* ROLLINGAV_H_ */
