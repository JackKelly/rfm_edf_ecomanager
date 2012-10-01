/*
 * Manager.cpp
 *
 *  Created on: 26 Sep 2012
 *      Author: jack
 */

#include "Manager.h"
#include "consts.h"

Sensor::Sensor()
:uid(UID_INVALID), watts0(WATTS_INVALID), eta(0)
{}

void Sensor::update(const Packet& packet)
{
	eta = packet.get_timecode() + SAMPLE_PERIOD;

	const uint16_t* watts = packet.get_watts();
	watts0 = watts[0];
}

const unsigned long Sensor::get_eta() const
{
	return eta;
}

void Sensor::missing()
{
	eta += SAMPLE_PERIOD;
	Serial.print(millis());
	Serial.print("uid:");
	Serial.print(uid);
	Serial.print(" is missing. new eta = ");
	Serial.println(eta);
}

void Sensor::set_uid(const uint32_t& _uid)
{
	uid = _uid;
}

const uint32_t Sensor::get_uid() const
{
	return uid;
}

WholeHouseTx::WholeHouseTx()
:Sensor()
{
	watts1 = watts2 = WATTS_INVALID;
}

void WholeHouseTx::print() const
{
	Serial.print(millis());
	Serial.print("{uid:");
	Serial.print(uid);
	if (watts0!=WATTS_INVALID) {
		Serial.print(", s0:");
		Serial.print(watts0);
	}
	if (watts1!=WATTS_INVALID) {
		Serial.print(", s1:");
		Serial.print(watts1);
	}
	if (watts2!=WATTS_INVALID) {
		Serial.print(", s2:");
		Serial.print(watts2);
	}
	Serial.println("}");

}


void WholeHouseTx::update(const Packet& packet)
{
	Sensor::update(packet);

	const uint16_t* watts = packet.get_watts();
	watts1 = watts[1];
	watts2 = watts[2];
	print();
}


Manager::Manager()
: next_iam(0), next_expected_tx(0)
{
	// TODO: this stuff needs to be programmed over serial not hard-coded.
	num_whole_house_txs = 2;
	whole_house_txs[0].set_uid(895);
	whole_house_txs[1].set_uid(28);

	num_iams = 1;
	iam_ids[0] = 3;
}

void Manager::init()
{
	update_next_expected_tx();
    rfm.init_edf();
    rfm.enable_rx();

    uint32_t uid;
    uint8_t index;

    // listen for a while to catch the timings of the whole_house transmitters
    Serial.println("Passively listening for 30s...");
    const unsigned long start_time = millis();
    while (millis() < (start_time+30000)) {
    	if (rfm.rx_packet_buffer.valid_data_is_available()) {
			for (uint8_t i=0; i<rfm.rx_packet_buffer.current_packet; i++) {
				uid = rfm.rx_packet_buffer.packets[i].get_uid();
				if (rfm.rx_packet_buffer.packets[i].is_ok()) {
					index = find_index_given_uid(uid);
					if (index!=INVALID_INDEX) {
						whole_house_txs[index].update(
								rfm.rx_packet_buffer.packets[i]);
					}
				}
			}
			if (rfm.rx_packet_buffer.packets[rfm.rx_packet_buffer.current_packet].done()) {
				Serial.print(millis());
				Serial.println(" DANGER: LOOSING DATA!");
			}
			rfm.rx_packet_buffer.reset_all();
    	}
    }
    Serial.println("...done passively listening.");
    update_next_expected_tx();

}

const uint8_t Manager::find_index_given_uid(const uint32_t& uid)
{
	for (uint8_t i=0; i<num_whole_house_txs; i++) {
		if (whole_house_txs[i].get_uid() == uid) {
			return i;
		}
	}
	Serial.print(millis());
	Serial.print(" invalid index=");
	Serial.println(uid);

	return INVALID_INDEX;
}

void Manager::run()
{
	if (num_whole_house_txs == 0) {
		poll_next_iam();
	} else {
		if (millis() < (whole_house_txs[next_expected_tx].get_eta()-EARLY_OPEN)) {
			poll_next_iam();
		} else  {
			wait_for_whole_house_tx();
		}
	}
}

void Manager::poll_next_iam()
{
	// TODO
	// don't repeatedly poll iams; wait 6 seconds;
	// if we've finished polling iams for this period then go into listening
	// mode and respond immediately we get data.
	// if we accidentally catch a whole-house TX then update_next_expected_tx();
}

void Manager::wait_for_whole_house_tx()
{
	const unsigned long start_time = millis();

	uint8_t index;
	uint32_t uid;

	// TODO handle roll-over over millis().

	// listen for WHOLE_HOUSE_TX for defined period.
	Serial.print(millis());
	Serial.println(" Window open!");
	bool success = false;
	while (millis() < (start_time+WINDOW) && !success) {
		if (rfm.rx_packet_buffer.valid_data_is_available()) {
			for (uint8_t i=0; i<rfm.rx_packet_buffer.current_packet; i++) {
				uid = rfm.rx_packet_buffer.packets[i].get_uid();
				if (rfm.rx_packet_buffer.packets[i].is_ok()) {
					if (uid == whole_house_txs[next_expected_tx].get_uid()) {
						success = true;
						index = next_expected_tx;
					} else {
						index = find_index_given_uid(uid);
					}
					if (index!=INVALID_INDEX) {
						whole_house_txs[index].update(
								rfm.rx_packet_buffer.packets[i]);
					}
				} else {
					Serial.print(millis());
					Serial.print(" Broken packet received. uid=");
					Serial.println(uid);
				}
			}
			if (rfm.rx_packet_buffer.packets[rfm.rx_packet_buffer.current_packet].done()) {
				Serial.print(millis());
				Serial.println(" DANGER: LOOSING DATA!");
			}
			rfm.rx_packet_buffer.reset_all();
		}
	}

	Serial.print(millis());
	Serial.print(" window closed. success=");
	Serial.println(success);

	if (!success) {
		// tell whole-house TX it missed its slot
		whole_house_txs[next_expected_tx].missing();
	}

	update_next_expected_tx();
}

void Manager::update_next_expected_tx()
{
	for (uint8_t i=0; i<num_whole_house_txs; i++) {
		if (whole_house_txs[i].get_eta() < whole_house_txs[next_expected_tx].get_eta()) {
			next_expected_tx = i;
		}
	}
	Serial.print(millis());
	Serial.print(" Next expected tx has uid = ");
	Serial.print(whole_house_txs[next_expected_tx].get_uid());
	Serial.print(" eta=");
	Serial.println(whole_house_txs[next_expected_tx].get_eta());
}
