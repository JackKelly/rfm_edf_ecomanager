#include "consts.h"
#include "Logger.h"
#include "CcTx.h"

/**************************
 * CcTrx                  *
 **************************/

CcTrx::CcTrx(): id(ID_INVALID), active(true) {}


CcTrx::CcTrx(const id_t& _id): id(_id), active(true) {}


CcTrx::~CcTrx() {}


void CcTrx::print() const
{
    Serial.print(F("{\"id\": "));
    Serial.print(id);
    Serial.print(F(", \"active\": "));
    Serial.print(active);
    Serial.print(F("}"));
}

bool CcTrx::is_active() const
{
    return active;
}

/*************************
 * CcTx                  *
 *************************/

CcTx::CcTx():CcTrx() { init(); }


CcTx::CcTx(const id_t& _id): CcTrx(_id) { init(); }


void CcTx::init()
{
    /* This init() function could be avoided if
     * we use C++11, but I can't think of any way to
     * force the Arduino IDE to use C++11.
     * http://stackoverflow.com/questions/308276/c-call-constructor-from-constructor */
    eta = 0xFFFFFFFF;
    num_periods_missed = 0;
    last_seen = 0;
}


CcTx::~CcTx() {}


void CcTx::print()
{
    Serial.print(F("{\"id\": "));
    Serial.print(id);
    Serial.print(F(", \"last_seen\": "));
    Serial.print(last_seen);
    Serial.print(F(", \"num_periods_missed\": "));
    Serial.print(num_periods_missed);
    Serial.print(F(", \"eta\": "));
    Serial.print(eta);
    Serial.print(F(", \"sample_period\": "));
    Serial.print(sample_period.get_av());
    Serial.print(F(", \"active\": "));
    Serial.print(active);
    Serial.print(F("}"));
}


void CcTx::update(const RXPacket& packet)
{
    if (last_seen != 0) {
        uint16_t new_sample_period;

        // update sample period for this Sensor
        // (seems to vary a little between sensors)
        log(DEBUG, PSTR("TX %lu old sample period=%u"), id, sample_period.get_av()); /* Old sample period */

        new_sample_period = (packet.get_timecode() - last_seen) / num_periods_missed;

        // Check the new sample_period is sane
        if (new_sample_period > 5700 && new_sample_period < 6300) {
            log(DEBUG, PSTR("Adding new_sample_period %u"), new_sample_period); /* Adding new sample period */
            sample_period.add_sample(new_sample_period);
        }

        log(DEBUG, PSTR("TX %lu new sample period=%u"), id, sample_period.get_av()); /* New sample period */
    }
	eta = packet.get_timecode() + sample_period.get_av();
	num_periods_missed = 1;
	last_seen = packet.get_timecode();
	active = true;
}


const id_t& CcTx::get_eta()
{
    // Sanity-check ETA to make sure it's in the future.
    if (eta+CC_TX_WINDOW_OPEN < millis() &&
            eta+CC_TX_WINDOW_OPEN+SAMPLE_PERIOD < millis()+SAMPLE_PERIOD)
        /* one possible reason
            for eta < millis() is that millis() has rolled over.
            If millis() has rolled over
            then eta+SAMPLE_PERIOD > millis()+SAMPLE_PERIOD.  In other words, only
            called missing() if the fact that eta < millis cannot be explained
            by roll-over.  We want to let roll-over do its thing.  */
    {
        log(DEBUG, PSTR("eta %lu < millis() %lu. id=%lu. num_periods=%d, active=%d"), eta, millis(), id, num_periods_missed, active);
        missing();
    }
	return eta;
}


void CcTx::missing()
{
	eta += sample_period.get_av();
	num_periods_missed++;

	if (num_periods_missed > 5) {
	    active = false;
	}

	log(INFO, PSTR("id:%lu missing. New ETA=%lu missed=%u"), id, eta, num_periods_missed);
}


/******************************
 * CcTxArray                  *
 ******************************/

void CcTxArray::next()
{
    for (index_t j=0; j<size; j++) {
        if (data[j].active && data[j].get_eta() < current().get_eta()) {
            i = j;
        }
    }
    log(DEBUG, PSTR("Next TX ID=%lu, ETA=%lu"),
            current().id, current().get_eta());
}


void CcTxArray::print_name() const
{
    Serial.print(F(" CC_TX"));
}


/******************************
 * CcTrxArray                 *
 ******************************/

void CcTrxArray::next()
{
    i++;
    if (i==size) i=0;
}


void CcTrxArray::print_name() const
{
    Serial.print(F(" CC_TRX"));
}
