/*
 * DynamicArray.h
 *
 *  Created on: 16 Oct 2012
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

#ifndef DYNAMICARRAY_H_
#define DYNAMICARRAY_H_

#ifdef TESTING
#include "tests/FakeArduino.h"
#else
#include <Arduino.h>
#undef max // Arduino pollutes the namespace with these min and max macros which breaks compilation
#undef min //
#include "Logger.h"
#include "new_fix.h"
#endif

#include "consts.h"
#include "utils.h"

/**
 * A DynamicArray template for storing multiple CcTx or CcTrx objects.
 * Keeps objects in order of ID to make searching for IDs fast (because
 * searching happens very frequently).
 * Appending items to the list happens very rarely so it's OK to make
 * append operations quite costly.
 * To minimise memory fragmentation, it is best to allocate space using
 * set_size(index_t) prior to appending data to the array using append(id_t).
 * However, append(id_t) will allocate more space if n == size when append(id_t)
 * is called.
 */
template <class item_t>
class DynamicArray {
protected:
    item_t * data;
    index_t size, /* total number of allocated slots */
            i,    /* index of the "current" item */
            n;    /* number of items currently stored */
    id_t    min_id, max_id; /* used to speed up search */

public:
    DynamicArray()
    : data(0), size(0), i(0), n(0), min_id(0), max_id(0) {}


    virtual ~DynamicArray()
    {
        delete[] data;
    }


    /* Copy Constructor
     * (compile with -fno-elide-constructors to see this in action when copying
     *  a DynamicArray object back from a function) */
    DynamicArray(const DynamicArray& src)
    : size(src.size), i(src.size), n(src.n),
      min_id(src.min_id), max_id(src.max_id)
    {
        data = new item_t[size];
        if (data) {
            src.copy(data, 0, 0, n);
        } else {
            log(ERROR, PSTR("OUT OF MEMORY"));
        }
    }


    DynamicArray<item_t>& operator=(const DynamicArray& src)
    {
        if (data) { /* check if we're overwriting some old data */
            delete [] data;
        }

        size   = src.size;
        i      = src.i;
        n      = src.n;
        min_id = src.min_id;
        max_id = src.max_id;

        data = new item_t[size];
        if (data) {
            src.copy(data, 0, 0, n);
        } else {
            log(ERROR, PSTR("OUT OF MEMORY"));
        }
        return *this;
    }


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
    item_t& operator[](const index_t& index)
    {
        if (data && index < n) {
            return data[index];
        } else {
            log(WARN, PSTR("DYNAMIC ARRAY OUT OF RANGE ERROR"));
        }
    }


    const item_t& operator[](const index_t& index) const
    {
        if (data && index < n) {
            return data[index];
        } else {
            log(WARN, PSTR("DYNAMIC ARRAY OUT OF RANGE ERROR"));
        }
    }
#pragma GCC diagnostic pop


    const index_t& get_n() const { return n; }


    const index_t& get_i() const { return i; }


    item_t& current() { return data[i]; }


    virtual void print_name() const = 0;


    bool set_size(const index_t& new_size)
    {
        item_t* new_data = new item_t[new_size];
        if (new_data) {
            size = new_size;
        } else {
            log(WARN, PSTR("DYNAMIC ARRAY OUT OF MEMORY"));
            return false;
        }

        copy(new_data, 0, 0, n); // won't do anything if n==0
        delete [] data;
        data = new_data;
        return true;
    }


#ifndef TESTING
    void set_size_from_serial()
    {
        Serial.print(F("ACK enter number of "));
        print_name();
        Serial.println(F("s:"));

        uint32_t new_size = utils::read_uint32_from_serial();

        bool success;

        success = new_size == UINT32_INVALID ? false : set_size(new_size);

        Serial.print(success ? F("ACK") : F("NAK not"));
        Serial.print(F(" added "));
        Serial.print(new_size);
        print_name();
        Serial.println(F("s"));
    }


    void get_id_from_serial()
    {
        Serial.print(F("ACK enter "));
        print_name();
        Serial.println(F(" ID to add:"));

        uint32_t id = utils::read_uint32_from_serial();

        bool success;
        success = id == UINT32_INVALID ? false : append(id);

        Serial.print(success ? F("ACK") : F("NAK not"));
        Serial.print(F(" added "));
        print_name();
        Serial.print(F(" "));
        Serial.println(id);
    }


    void remove_id_from_serial()
    {
        Serial.print(F("ACK enter "));
        print_name();
        Serial.println(F(" ID to remove:"));

        uint32_t id = utils::read_uint32_from_serial();

        bool success;
        success = id == UINT32_INVALID ? false : remove_id(id);

        Serial.print(success ? F("ACK") : F("NAK not"));
        Serial.print(F(" removed "));
        print_name();
        Serial.print(F(" "));
        Serial.println(id);
    }
#endif // TESTING


    bool remove_index(const index_t& index)
    {
        if (index >= n) {
            Serial.println(F("NAK index too large"));
            return false;
        }

        /* Copy contents down one (can't use copy() because it goes backwards) */
        const index_t length = (n-index) - 1;
        const index_t src_start = index+1;
        for (index_t j=0; j<length; j++) {
            data[j+index] = data[j+src_start];
        }

        /* Update min_id or max_id if we removed first or last entry, respectively */
        if (index == 0) {
            min_id = data[0].id;
        } else if (index == n-1) {
            max_id = data[n-2].id;
        }

        n--;
        return true;
    }


    bool remove_id(const id_t& id)
    {
        index_t index = 0;

        if (find(id, index)) {
            return remove_index(index);
        } else {
            log(DEBUG, PSTR("%lu not in data."), id);
            return false;
        }
    }


    bool append(const id_t& id)
    {
        index_t upper_bound = 0;

        if (find(id, upper_bound)) {
            log(DEBUG, PSTR("%lu is already in data."), id);
            return false;
        }

        if (n < size) {
            /* so just move items from upper_bound to size up
             * 1 position to keep array sorted after appending new item */
            copy(data, upper_bound, upper_bound+1, n-upper_bound);
            data[upper_bound] = item_t(id);
            n++;
        } else { // n == size so allocate more memory
            item_t * new_data = new item_t[size+1];
            if (new_data == 0) {
                log(ERROR, PSTR("OUT OF MEMORY"));
                return false;
            }

            copy(new_data, 0, 0, upper_bound);
            new_data[upper_bound] = item_t(id);
            copy(new_data, upper_bound, upper_bound+1, n-upper_bound);

            delete[] data;
            data = new_data;
            n = ++size;
        }

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
        // copy backwards so shifting contents
        // upwards 1 place works
        for (index_t i=length-1; i<length; i--) { // termination condition is i<length because i is unsigned
            dst[i+dst_start] = data[i+src_start];
        }
    }


    /* Entry point for find when called with just target_id */
    bool find(const id_t& target_id) const
    {
        index_t index = 0;
        return find(target_id, index);
    }


    /** Attempts to find target ID in data.
     *  If target can't be found then returns false and index == upper_bound nearest target.
     *  Note that if we search for an ID that's above the largest ID in index will be
     *  equal to n. */
    bool find(const id_t& target_id, index_t& index) const
    {

        if (target_id < min_id) {
            index = 0;
            return false;
        } else if (target_id > max_id) {
            index = n;
            return false;
        }
        else if (n > 1) {
            // Try to guess the candidate item position by looking at the size of target_id
            // relative to difference of max - min.  This will give us an educated guess and
            // then we search backwards or forwards from that starting guess.
            float diff = max_id - min_id;
            index = ((target_id - min_id) / diff) * (n-1);

            if (index >= n) index = n;

            // Search backwards
            for (; index > 0 && data[index].id > target_id; index--)
                ;

            // Search forwards
            for (; index < n && data[index].id < target_id; index++)
                ;

            return data[index].id == target_id;
        }
        else if (n == 1) {
            if (target_id == operator[](0).id) {
                index = 0;
                return true;
            }
        }
        return false; // should never get here (this is just to keep the compiler happy)
    }


    /* Don't de-allocate memory; just set n and i to 0 */
    void delete_all()
    {
        n = i = 0;
        min_id = max_id = 0;
        Serial.print(F("ACK deleted all "));
        print_name();
        Serial.println(F("s"));
    }


    void print() const
    {
        Serial.println(F("ACK"));
        Serial.print(F("{\""));
        print_name();
        Serial.println(F("s\": ["));

        for (index_t i=0; i<n; i++) {
            data[i].print();
            if (i < n-1) {
                Serial.println(F(","));
            }
        }

        Serial.println(F("\r\n]}"));
    }
};

#endif /* DYNAMICARRAY_H_ */
