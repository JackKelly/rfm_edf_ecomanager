#include "Sensor.h"
#include "consts.h"
#include "debug.h"

Sensor::Sensor()
:eta(0xFFFFFFFF), id(ID_INVALID), sample_period(SAMPLE_PERIOD),
 num_periods(0), time_last_seen(0)
{}

void Sensor::update(const RXPacket& packet)
{
    if (time_last_seen != 0) {
        // update sample period for this Sensor
        // (seems to vary a little between sensors)

        debug(INFO, "CC_TX ID=%lu, old sample_period=%u", id, sample_period);

        sample_period = (packet.get_timecode() - time_last_seen) / num_periods;

        // Check the new sample_period is sane
        if (sample_period < 5700 || sample_period > 6300) {
            sample_period = SAMPLE_PERIOD;
        }

        debug(INFO, "CC_TX ID=%lu, new sample_period=%u", id, sample_period);
    }
	eta = packet.get_timecode() + sample_period;
	num_periods = 1;
	time_last_seen = packet.get_timecode();
}

const uint32_t& Sensor::get_eta() const
{
	return eta;
}

void Sensor::missing()
{
	eta += sample_period;
	num_periods++;

	debug(INFO, "id:%lu is missing. New ETA=%lu, num_periods missed=%u", id, eta, num_periods);
}

void Sensor::set_id(const uint32_t& _id)
{
	id = _id;
}

const uint32_t& Sensor::get_id() const
{
	return id;
}
