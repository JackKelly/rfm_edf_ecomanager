/*
 * Manager.cpp
 *
 *  Created on: 26 Sep 2012
 *      Author: jack
 */

#include "Manager.h"
#include "consts.h"

Sensor::Sensor()
:eta(0), uid(UID_INVALID)
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


Manager::Manager()
: i_of_next_expected_tx(0), next_iam(0), retries(0), timecode_polled_first_iam(0)
{
	// TODO: this stuff needs to be programmed over serial not hard-coded.
	num_whole_house_txs = 2;
	whole_house_txs[0].set_uid(895);
	whole_house_txs[1].set_uid(28);

	num_iams = 1;
	iam_ids[0] = 0x55100003;
}

void Manager::init()
{
	find_next_expected_tx();
    rfm.init_edf();
    rfm.enable_rx();

    // listen for a while to catch the timings of the whole_house transmitters
    Serial.print(millis());
    Serial.println(" Passively listening for 30s...");

    const unsigned long start_time = millis();
    while (millis() < (start_time+30000)) {
    	if (rfm.rx_packet_buffer.valid_data_is_available()) {
			process_rx_packet_buffer(0);
    	}
    }

    Serial.print(millis());
    Serial.println(" ...done passively listening.");
    find_next_expected_tx();
}

void Manager::process_whole_house_uid(const uint32_t& uid, const RXPacket& packet)
{
	for (uint8_t i=0; i<num_whole_house_txs; i++) {
		if (whole_house_txs[i].get_uid() == uid) {
			whole_house_txs[i].update(packet);
			break;
		}
	}
}

void Manager::run()
{
	if (num_whole_house_txs == 0) {
		poll_next_iam();
	} else {
		if (millis() < (whole_house_txs[i_of_next_expected_tx].get_eta()-(WINDOW/2))) {
			poll_next_iam();
		} else  {
			wait_for_whole_house_tx();
		}
	}
}

void Manager::poll_next_iam()
{
	// don't repeatedly poll iams; wait SAMPLE_PERIOD seconds;
	if (next_iam==0) {
		if (millis() < timecode_polled_first_iam+SAMPLE_PERIOD && retries==0) {
			return;
		} else {
			timecode_polled_first_iam = millis();
		}
	}

	Serial.print(millis());
	Serial.print(" polling IAM ");
	Serial.println(iam_ids[next_iam]);
	rfm.poll_edf_iam(iam_ids[next_iam]);

	// wait for response
	const unsigned long start_time = millis();
	bool success = false;
	while (millis() < start_time+IAM_TIMEOUT) {
		if (rfm.rx_packet_buffer.valid_data_is_available()
				&& process_rx_packet_buffer(iam_ids[next_iam])) {
			success = true;
			break;
		}
	}

	if (success) {
		increment_next_iam();
	} else {
		if (retries < MAX_RETRIES) {
			retries++;
		} else {
			increment_next_iam();
		}
	}
}

void Manager::increment_next_iam()
{
	next_iam++;
	if (next_iam >= num_iams) {
		next_iam=0;
	}

	retries = 0;
}

void Manager::wait_for_whole_house_tx()
{
	const unsigned long start_time = millis();

	// TODO handle roll-over over millis().

	// listen for WHOLE_HOUSE_TX for defined period.
	Serial.print(millis());
	Serial.println(" Window open!");
	bool success = false;
	while (millis() < (start_time+WINDOW) && !success) {
		if (rfm.rx_packet_buffer.valid_data_is_available() &&
				process_rx_packet_buffer(whole_house_txs[i_of_next_expected_tx].get_uid())) {
			success = true;
		}
	}

	Serial.print(millis());
	Serial.print(" window closed. success=");
	Serial.println(success);

	if (!success) {
		// tell whole-house TX it missed its slot
		whole_house_txs[i_of_next_expected_tx].missing();
	}

	find_next_expected_tx();
}

const bool Manager::process_rx_packet_buffer(const uint32_t& target_uid)
{
	bool success = false;
	uint32_t uid;
	RXPacket* packet = NULL;

	for (uint8_t packet_i=0; packet_i<=rfm.rx_packet_buffer.current_packet; packet_i++) {
		packet = &rfm.rx_packet_buffer.packets[packet_i];
		if (packet->done()) {
			if (packet->is_ok()) {
				packet->print_uid_and_watts(); // send data over serial
				uid = packet->get_uid();
				success = (uid == target_uid);

				if (!uid_is_iam(uid)) {
					process_whole_house_uid(uid, *packet);
				}

			} else {
				Serial.print(millis());
				Serial.println(" Broken packet received.");
			}
		}
	}

	rfm.rx_packet_buffer.reset_all();
	return success;
}

const bool Manager::uid_is_iam(const uint32_t& uid) const
{
	for (uint8_t i=0; i<num_iams; i++) {
		if (iam_ids[i] == uid) {
			return true;
		}
	}
	return false;
}

void Manager::find_next_expected_tx()
{
	for (uint8_t i=0; i<num_whole_house_txs; i++) {
		if (whole_house_txs[i].get_eta() < whole_house_txs[i_of_next_expected_tx].get_eta()) {
			i_of_next_expected_tx = i;
		}
	}
	Serial.print(millis());
	Serial.print(" Next expected tx has uid = ");
	Serial.print(whole_house_txs[i_of_next_expected_tx].get_uid());
	Serial.print(" eta=");
	Serial.println(whole_house_txs[i_of_next_expected_tx].get_eta());
}
