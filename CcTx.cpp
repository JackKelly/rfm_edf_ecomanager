#include "CcTx.h"
#include "consts.h"
#include "Logger.h"

CcTrx::CcTrx()
: id(ID_INVALID)
{}

CcTx::CcTx()
:CcTrx(), eta(0xFFFFFFFF), sample_period(SAMPLE_PERIOD),
 num_periods(0), time_last_seen(0)
{}


void CcTx::update(const RXPacket& packet)
{
    if (time_last_seen != 0) {
        // update sample period for this Sensor
        // (seems to vary a little between sensors)
        // TODO: take an average for the sample period
        log(DEBUG, "CC_TX ID=%lu, old sample_period=%u", id, sample_period);

        sample_period = (packet.get_timecode() - time_last_seen) / num_periods;

        // Check the new sample_period is sane
        if (sample_period < 5700 || sample_period > 6300) {
            sample_period = SAMPLE_PERIOD;
        }

        log(DEBUG, "CC_TX ID=%lu, new sample_period=%u", id, sample_period);
    }
	eta = packet.get_timecode() + sample_period;
	num_periods = 1;
	time_last_seen = packet.get_timecode();
}


const uint32_t& CcTx::get_eta()
{
    // Sanity-check ETA to make sure it's in the future.
    if (eta < millis()) {
        eta = 0xFFFFFFFF;
    }
	return eta;
}


void CcTx::missing()
{
	eta += sample_period;
	num_periods++;

	log(INFO, "id:%lu is missing. New ETA=%lu, num_periods missed=%u", id, eta, num_periods);
}
