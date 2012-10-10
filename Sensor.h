#ifndef SENSOR_H
#define SENSOR_H

#include "Packet.h"

// TODO: tweak SAMPLE_PERIOD per Sensor based on experience (each transmitter
// seems to have a slightly different transmit period).
class Sensor {
public:
	Sensor();
	void update(const RXPacket& packet);
	void missing();
	void set_uid(const uint32_t& _uid);
	const uint32_t get_uid() const;
	const unsigned long get_eta() const;

protected:
	unsigned long eta; // estimated time of arrival in miliseconds since power-on
	uint32_t uid;
};

#endif
