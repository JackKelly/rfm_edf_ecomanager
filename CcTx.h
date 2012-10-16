#ifndef SENSOR_H
#define SENSOR_H

#include "Packet.h"

/*
void * operator new[](size_t size)
{
    return malloc(size);
}

void operator delete[](void * ptr)
{
    free(ptr);
}
*/

//TODO: try this on Windows Arduino IDE
//TODO: install avr-gcc 4.7.2

/**
 * Class for Current Cost / EDF Transceiver (TRX) units
 */
class CcTrx {
public:
    CcTrx();
    uint32_t id; /* Deliberately public */

    void update(const RXPacket& packet) {int * a = new int[10];}
    void missing() {};
    const uint32_t& get_eta() {return 0;}
};

/**
 * Class for Current Cost Transmit-Only units
 */
class CcTx : public CcTrx {
public:
	CcTx();
	void update(const RXPacket& packet);
	void missing();
	const uint32_t& get_eta();

protected:
	uint32_t eta; // estimated time of arrival in milliseconds since power-on
	uint16_t sample_period;
	uint8_t  num_periods;
	uint32_t time_last_seen;
};

// TODO: re-write this using template so it does the right thing for find etc.

/**
 * Class for storing multiple CcTrxs or CcTxs
 */
class CcArray {
public:
    /********************************
     * Public variables             *
     ********************************/
    CcTrx* cc_array;

    /********************************
     * Public methods               *
     ********************************/
    CcArray(const char* _name)
    : cc_array(NULL), length(0), n(0), i(0), cached_id(0), cached_result(NULL),
      name(_name)
    {}

    /**
     * Changes i to be an index to the next item with the result
     * that current() returns the next item.
     */
    virtual void next() = 0;

    /**
     * @return pointer to entry with id
     * or null if id is not found.
     */
    CcTrx* find(const uint32_t& id);

    /**
     * Try to add an item to cc_array. First checks to see if
     * item is already in array (if it is then just return false).
     * @return true if item is added to list.
     */
    const bool append(const uint32_t id);

    const uint8_t get_n() const {return n;}
    const uint8_t get_i() const {return i;}
    void set_length(const uint8_t _length) {length=_length;}

protected:
    uint8_t length, n, i;
    uint32_t cached_id;
    CcTrx* cached_result;
    const char* name;
};

class CcTxArray : public CcArray {
public:

    /********************************
     * Public methods               *
     ********************************/
    CcTxArray(): CcArray("CC_TX") {}
    CcTx* current();
    void next();
};

class CcTrxArray : public CcArray {
public:

    /********************************
     * Public methods               *
     ********************************/
    CcTrxArray(): CcArray("CC_TX") {}
    CcTrx* current();
    void next();
};

#endif
