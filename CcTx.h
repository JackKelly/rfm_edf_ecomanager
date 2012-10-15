#ifndef SENSOR_H
#define SENSOR_H

#include "Packet.h"

/**
 * Class for Current Cost / EDF Transceiver (TRX) units
 */
class CcTrx {
public:
    CcTrx();
    uint32_t id; /* Deliberately public */
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

#endif
