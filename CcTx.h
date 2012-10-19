#ifndef SENSOR_H
#define SENSOR_H

#ifdef TESTING
#include "tests/FakeArduino.h"
#else
#include <Arduino.h>
#include "new.h"
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

    id_t id; /* Deliberately public */
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
	uint8_t  num_periods;
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
