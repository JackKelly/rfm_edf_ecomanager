#include "consts.h"
#include "Logger.h"
#include "CcTx.h"

CcTrx::CcTrx(): id(ID_INVALID) {}


CcTrx::CcTrx(const uint32_t& _id): id(_id) {}


CcTx::CcTx():CcTrx() { init(); }


CcTx::CcTx(const uint32_t& _id): CcTrx(_id) { init(); }


void CcTx::init()
{
    // use of this init() function could be avoided if
    // we use C++11, but I can't think of any way to
    // force the Arduino IDE to use C++11.
    // http://stackoverflow.com/questions/308276/c-call-constructor-from-constructor
    eta = 0xFFFFFFFF;
    sample_period = SAMPLE_PERIOD;
    num_periods = 0;
    time_last_seen = 0;
}

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

/******************************
 * CcArray                    *
 ******************************/
/* TODO: remove if not needed
const bool CcArray::append(const uint32_t id)
{
    log(DEBUG, "CcArray::append(%lu). n=%d", id, n);

    if (find(id)) {
        log(DEBUG, "%s %lu already in list", name, id);
        return false;
    }

    if (n < length) {
        cc_array[n++].id = id;
        log(DEBUG, "Added %s id %lu", name, cc_array[n-1].id);
        return true;
    } else {
        log(ERROR, "no space for %s %lu", name, id);
        return false;
    }
}

CcTrx* CcArray::find(const uint32_t& id)
{
    for (uint8_t il=0; il<n; il++) {
        if (cc_array[il].id == id) {
            return &cc_array[il];
        }
    }
    return NULL;
}
*/

/******************************
 * CcTxArray                    *
 ******************************/


void CcTxArray::next()
{
    for (uint8_t j=0; j<size; j++) {
        if (data[j].get_eta() < current().get_eta()) {
            i = j;
        }
    }
    log(DEBUG, "Next expected CC_TX: ID=%lu, ETA=%lu",
            current().id, current().get_eta());
}

/******************************
 * CcTrxArray                    *
 ******************************/


void CcTrxArray::next()
{
    i++;
    if (i==size) i=0;
}
