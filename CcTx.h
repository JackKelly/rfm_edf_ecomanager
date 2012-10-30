/*
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

#ifndef SENSOR_H
#define SENSOR_H

#ifdef TESTING
#include "tests/FakeArduino.h"
#else
//#include <Arduino.h>
#endif

#include "Packet.h"
#include "DynamicArray.h"
#include "RollingAv.h"

/**
 * Class for Current Cost / EDF Transceiver (TRX) units
 */
class CcTrx {
public:
    CcTrx();
    CcTrx(const id_t& _id);
    virtual ~CcTrx();
    virtual void print() const;
    bool is_active() const;

    id_t id; /* Deliberately public */
    bool active;
};

/**
 * Class for Current Cost Transmit-Only units
 */
class CcTx : public CcTrx {
public:
	CcTx();
	CcTx(const id_t& _id);
	~CcTx();
	void update(const RXPacket& packet);
	void missing();
	const millis_t& get_eta();
	void print();

protected:
	void init(); // called from constructors
	millis_t eta; // estimated time of arrival in milliseconds since power-on
	RollingAv sample_period;
	uint8_t  num_periods_missed;
	millis_t last_seen;
};


class CcTxArray : public DynamicArray<CcTx> {
public:
    void next();
    void print_name() const;
};

class CcTrxArray : public DynamicArray<CcTrx> {
public:
    void next();
    void print_name() const;
};

#endif /* SENSOR_H */
