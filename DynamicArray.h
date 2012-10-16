/*
 * DynamicArray.h
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#ifndef DYNAMICARRAY_H_
#define DYNAMICARRAY_H_

#ifdef TESTING
#include "tests/FakeArduino.h"
#else
#include <Arduino.h>
#include "Logger.h"
#endif


#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif

// TODO: add remove() function to remove ID
// TODO: remove commented-out calls to log()

template <class item_t>
class DynamicArray {
private:
    item_t * data;
    uint8_t size;
    item_t min, max;

public:
    DynamicArray(): size(0), min(0), max(0)
    {
        data = new item_t[size];
    }


    ~DynamicArray()
    {
        delete[] data;
    }


    // Copy Constructor (compile with -fno-elide-constructors to see this in action)
    DynamicArray(const DynamicArray& src): size(src.size), min(src.min), max(src.max)
    {
        std::cout << "copy constructor DynamicArray(const DynamicArray& src)" << std::endl;
        data = new item_t[size];
        src.copy(data, 0, 0, size);
    }


    item_t& operator[](const uint8_t& index)
    {
        if (index < size) {
            return data[index];
        } else {
            log(WARN, "DYNAMIC ARRAY OUT OF RANGE ERROR");
            return data[0];
        }
    }


    bool append(const item_t item)
    {
        uint8_t upper_bound = 0;

        if (find(item, &upper_bound)) {
            log(DEBUG, "%lu is already in data.", item);
            print();
            return false;
        }

        item_t * new_data = new item_t[size+1];

        copy(new_data, 0, 0, upper_bound);
        new_data[upper_bound] = item;
        copy(new_data, upper_bound, upper_bound+1, size-upper_bound);

        delete[] data;
        data = new_data;
        size++;

        // Update min and max if necessary
        if (size==1) {
            min = max = item;
        } else {
            if (item > max) {
                max = item;
            } else if (item < min) {
                min = item;
            }
        }
    }

    /* copy data from this.data to dst */
    void copy(item_t * dst, const uint8_t src_start,
            const uint8_t dst_start, const uint8_t length) const
    {
        for (uint8_t i=0; i<length; i++) {
            dst[i+dst_start] = data[i+src_start];
        }
    }


    /** Attempts to find target in data.  If target can't be
     *  found then returns false and index == upper_bound nearest target. */
    const bool find(const item_t& target, uint8_t* index,
            const uint8_t lower_bound = 0, uint8_t upper_bound = UINT8_MAX) const
    {
        //log(DEBUG, "find(target=%lu, index=%d, lower_bound=%d, upper_bound=%d) size=%d",
        //        target, *index, lower_bound, upper_bound, size);

        uint8_t candidate_i;

        if (upper_bound == UINT8_MAX) { // UINT8_MAX is default so this is the initial call, not recursive call
            upper_bound = size;

            // Try to guess the candidate item position by looking at the size of target
            // relative to difference of max - min.
            item_t diff = max - min;
            if (diff == 0) { // avoid div by zero error
                return find(target, index, lower_bound, upper_bound);
            } else {
                if (target > max || target < min) {
                    *index = upper_bound;
                    return false;
                }
                candidate_i = ((target - min) / (float)diff) * (size-1);
                //log(DEBUG, "initial call.  candidate i=%d, min=%lu,"
                //        " max=%lu, diff=%lu, size=%d",
                //        candidate_i, min, max, diff, size);
            }

        } else { // This is a recursive call so look half way between upper_bound and lower_bound
            const uint8_t window = upper_bound - lower_bound;
            //log(DEBUG, " window=%d", window);
            if (window <= 1) { // target can't be found
                (*index) = upper_bound;
                return false;
            }
            candidate_i = (window / 2) + lower_bound;
        }

        item_t candidate = data[candidate_i];

        if (target == candidate) {
            *index = candidate_i;
            //log(DEBUG, "Target found. Target=%lu, candidate=%lu, index=%d", target, candidate, *index);
            return true;
        } else if (target < candidate) {
            return find(target, index, lower_bound, candidate_i);
        } else {
            return find(target, index, candidate_i, upper_bound);
        }
    }


    void print() const
    {
        for (uint8_t i=0; i<size; i++) {
            Serial.print(data[i]);
            Serial.print(" ");
        }
        Serial.println(" ");
    }
};


#endif /* DYNAMICARRAY_H_ */
