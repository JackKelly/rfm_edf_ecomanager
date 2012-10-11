#ifndef SENSOR_H
#define SENSOR_H

#include "Packet.h"

class Sensor {
public:
	Sensor();
	void update(const RXPacket& packet);
	void missing();
	void set_id(const uint32_t& _id);
	const uint32_t& get_id() const;
	const uint32_t& get_eta() const;

protected:
	uint32_t eta; // estimated time of arrival in milliseconds since power-on
	uint32_t id;
	uint16_t sample_period;
	uint8_t  num_periods;
	uint32_t time_last_seen;
};

#endif
