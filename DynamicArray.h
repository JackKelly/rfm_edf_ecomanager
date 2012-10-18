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
#undef max // Arduino pollutes the namespace with these min and max macros which breaks compilation
#undef min //
#include "Logger.h"
#include "new.h"
#endif

#include "consts.h"
#include "utils.h"

// TODO: add remove() function to remove ID
// TODO: implement a way to batch-add a specific number of items
// TODO: should be initialised to some size, then populated, then grown only if size limit is reached.

/**
 * A DynamicArray template for storing multiple CcTx and CcTrx objects.
 * Keeps objects in order of ID to make searching for IDs fast (because
 * searching happens very frequently).
 * Appending items to the list happens very rarely so it's OK to make
 * append operations quite costly.
 */
template <class item_t>
class DynamicArray {
protected:
    item_t * data;
    index_t size;
    index_t i; // index to the "current" item
    id_t    min_id, max_id;

public:
    DynamicArray()
    : data(0), size(0), i(0), min_id(0x0000), max_id(0x0000) {}


    ~DynamicArray()
    {
        delete[] data; // TODO: test that this doesn't blow up if data = NULL
    }


    // Copy Constructor (compile with -fno-elide-constructors to see this in action)
    DynamicArray(const DynamicArray& src)
    : size(src.size), i(src.size), min_id(src.min_id), max_id(src.max_id)
    {
        data = new item_t[size];
        if (data) {
            src.copy(data, 0, 0, size);
        } else {
            log(ERROR, "OUT OF MEMORY");
        }
    }

    DynamicArray<item_t>& operator=(const DynamicArray& src)
    {
        if (data) { // check if we're overwriting some old data
            delete [] data;
        }

        size = src.size;
        i = src.i;
        min_id = src.min_id;
        max_id = src.max_id;

        data = new item_t[size];
        if (data) {
            src.copy(data, 0, 0, size);
        } else {
            log(ERROR, "OUT OF MEMORY");
        }
        return *this;
    }

    item_t& operator[](const index_t& index)
    {
        if (data && index < size) {
            return data[index];
        } else {
            log(WARN, "DYNAMIC ARRAY OUT OF RANGE ERROR");
        }
    }


    const item_t& operator[](const index_t& index) const
    {
        if (data && index < size) {
            return data[index];
        } else {
            log(WARN, "DYNAMIC ARRAY OUT OF RANGE ERROR");
        }
    }


    const index_t get_size() const { return size; }

    const index_t get_i() const { return i; }

    item_t& current() { return data[i]; }

    const id_t get_id(const index_t index) { return operator[](index).id; }

    virtual void print_name() = 0;
/*
    bool grow(const index_t new_size)
    {
        if (size==0) {
            data = new item_t[new_size];
            if (data) {
                size = new_size;
                return true;
            } else {
                log(WARN, "DYNAMIC ARRAY OUT OF MEMORY");
                return false;
            }
        } else {
            // TODO
        }
    }
*/

    bool append(const id_t& id)
    {
        index_t upper_bound = 0;

        if (find(id, &upper_bound)) {
            log(DEBUG, "%lu is already in data.", id);
            print();
            return false;
        }

        item_t * new_data = new item_t[size+1];
        if (new_data == 0) {
            log(ERROR, "OUT OF MEMORY");
            return false;
        }

        copy(new_data, 0, 0, upper_bound);
        new_data[upper_bound] = item_t(id);
        copy(new_data, upper_bound, upper_bound+1, size-upper_bound);

        delete[] data;
        data = new_data;
        size++;

        // Update min_id and max if necessary
        if (size==1) {
            min_id = max_id = id;
        } else {
            if (id > max_id) {
                max_id = id;
            } else if (id < min_id) {
                min_id = id;
            }
        }

        return true;
    }


    /* copy data from this.data to dst */
    void copy(item_t * dst, const index_t src_start,
            const index_t dst_start, const index_t length) const
    {
        for (index_t i=0; i<length; i++) {
            dst[i+dst_start] = data[i+src_start];
        }
    }


    const bool find(const id_t& target_id) const
    {
        index_t index = 0;
        return find(target_id, &index);
    }


    /* Entry point to find() */
    const bool find(const id_t& target_id, index_t* index) const
    {
        index_t upper_bound = size, lower_bound = 0;

        if (size == 0) {
            *index = 0;
            return false;
        } else if (size == 1) {
            if (target_id == operator[](0).id) {
                *index = 0;
                return true;
            } else if (target_id < operator[](0).id) {
                *index = 0;
                return false;
            } else if (target_id > operator[](0).id) {
                *index = 1;
                return false;
            }
        } else { // size > 1
            if (target_id > max_id) {
                *index = size;
                return false;
            } else if (target_id < min_id) {
                *index = 0;
                return false;
            } else {
                // Try to guess the candidate item position by looking at the size of target_id
                // relative to difference of max - min.
                float diff = max_id - min_id;
                index_t candidate_i = ((target_id - min_id) / diff) * (size-1);
                return find_helper(target_id, index, lower_bound, upper_bound, candidate_i);
            }
        }
    }


    /** Attempts to find target ID in data.
     *  If target can't be found then returns false and index == upper_bound nearest target.
     *  Note that if we search for an ID that's above the largest ID in index will be
     *  equal to size. */
    const bool find(const id_t& target_id, index_t* index,
            const index_t lower_bound, index_t upper_bound) const
    {
        // Look half way between upper_bound and lower_bound
        const index_t window = upper_bound - lower_bound;
        if (window <= 1) { // target can't be found
            *index = upper_bound;
            return false;
        }
        index_t candidate_i = (window / 2) + lower_bound;

        return find_helper(target_id, index, lower_bound, upper_bound, candidate_i);
    }


    const bool find_helper(const id_t& target_id, index_t* index,
            const index_t lower_bound, index_t upper_bound, const index_t candidate_i) const
    {
        // TODO: replace operator[] call with data[] when fully debugged
        const id_t candidate_id = operator[](candidate_i).id;

        if (target_id == candidate_id) {
            *index = candidate_i;
            return true;
        } else if (target_id < candidate_id) {
            return find(target_id, index, lower_bound, candidate_i);
        } else {
            return find(target_id, index, candidate_i, upper_bound);
        }
    }


    void print() const
    {
        for (index_t i=0; i<size; i++) {
            Serial.print(data[i].id);
            Serial.print(" ");
        }
        Serial.println(" ");
    }


    void get_id_from_serial()
    {
        Serial.print("ACK enter ");
        print_name();
        Serial.println(" ID to add:");
        bool success;
        id_t id = utils::read_uint32_from_serial();
        success = append( id );
        Serial.print(success ? "ACK" : "NAK not");
        Serial.print(" added");
        print_name();
        Serial.println(id);
    }
};

#endif /* DYNAMICARRAY_H_ */
