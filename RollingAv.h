/*
 * RollingAv.h
 *
 *  Created on: 19 Oct 2012
 *      Author: jack
 */

#ifndef ROLLINGAV_H_
#define ROLLINGAV_H_

#include <inttypes.h>
#include "consts.h"

/**
 * Class for keeping a rolling average
 */
class RollingAv {
public:
    RollingAv();
    void add_sample(const uint16_t& sample);
    const uint16_t get_av();

private:
    static const index_t NUM_SAMPLES = 5;
    uint16_t samples[NUM_SAMPLES];
    index_t index;
    uint16_t av_cache;
    bool cache_valid;
};

#endif /* ROLLINGAV_H_ */
