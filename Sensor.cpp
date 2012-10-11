#include "Sensor.h"
#include "consts.h"

Sensor::Sensor()
:eta(0), id(ID_INVALID)
{}

void Sensor::update(const RXPacket& packet)
{
	eta = packet.get_timecode() + SAMPLE_PERIOD;
}

const unsigned long Sensor::get_eta() const
{
	return eta;
}

void Sensor::missing()
{
	eta += SAMPLE_PERIOD;
	Serial.print(millis());
	Serial.print(" uid:");
	Serial.print(id);
	Serial.print(" is missing. new eta = ");
	Serial.println(eta);
}

void Sensor::set_id(const uint32_t& _id)
{
	id = _id;
}

const uint32_t Sensor::get_id() const
{
	return id;
}
