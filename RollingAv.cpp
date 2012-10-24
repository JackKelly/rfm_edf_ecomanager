/*
 * RollingAv.cpp
 *
 *  Created on: 19 Oct 2012
 *      Author: jack
 */

#include "consts.h"
#include "RollingAv.h"

RollingAv::RollingAv(): index(0), av_cache(SAMPLE_PERIOD), cache_valid(true)
{
    for (index_t i=0; i<NUM_SAMPLES; i++) {
        samples[i] = SAMPLE_PERIOD;
    }
}


void RollingAv::add_sample(const uint16_t& sample)
{
    samples[index++] = sample;
    if (index == NUM_SAMPLES) {
        index = 0;
    }
    cache_valid = false;
}


const uint16_t RollingAv::get_av()
{
    if (!cache_valid) {
        uint16_t acc = 0;
        for (index_t i=0; i<NUM_SAMPLES; i++) {
            acc += samples[i];
        }
        av_cache = acc / NUM_SAMPLES;
        cache_valid = true;
    }
    return av_cache;
}
