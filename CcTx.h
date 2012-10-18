#ifndef SENSOR_H
#define SENSOR_H

#include "Packet.h"
#include "DynamicArray.h"

//TODO: try this on Windows Arduino IDE

/**
 * Class for Current Cost / EDF Transceiver (TRX) units
 */
class CcTrx {
public:
    CcTrx();
    CcTrx(const uint32_t& _id);
    id_t id; /* Deliberately public */
};

/**
 * Class for Current Cost Transmit-Only units
 */
class CcTx : public CcTrx {
public:
	CcTx();
	CcTx(const uint32_t& _id);
	void update(const RXPacket& packet);
	void missing();
	const uint32_t& get_eta();

protected:
	void init(); // called from constructors
	uint32_t eta; // estimated time of arrival in milliseconds since power-on
	uint16_t sample_period;
	uint8_t  num_periods;
	uint32_t time_last_seen;
};


class CcTxArray : public DynamicArray<CcTx> {
public:
    void next();
};

class CcTrxArray : public DynamicArray<CcTrx> {
public:
    void next();
};

#endif /* SENSOR_H */
