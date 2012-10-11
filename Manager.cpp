/*
 * Manager.cpp
 *
 *  Created on: 26 Sep 2012
 *      Author: jack
 */

#ifdef ARDUINO
#include <inttypes.h>
#endif

#include "Manager.h"
#include "consts.h"

Manager::Manager()
: p_next_cc_tx(cc_txs), i_next_cc_trx(0), retries(0), timecode_polled_first_cc_trx(0)
{
	// TODO: this stuff needs to be programmed over serial not hard-coded.
	num_cc_txs = 2;
	cc_txs[0].set_uid(895);
	cc_txs[1].set_uid(28);

	num_cc_trxs = 1;
	cc_trx_ids[0] = 0x55100003;
	id_next_cc_trx = cc_trx_ids[0];
}

void Manager::init()
{
	find_next_expected_cc_tx();
    rfm.init();
    rfm.enable_rx();

    // listen for a while to catch the timings of the cc tx transmit-only sensors
    Serial.print(millis());
    Serial.println(" Passively listening for 30s...");

    const unsigned long start_time = millis();
    while (millis() < (start_time+30000)) {
    	if (rfm.rx_packet_buffer.valid_data_is_available()) {
			process_rx_pack_buf_and_find_uid(0);
    	}
    }

    Serial.print(millis());
    Serial.println(" ...done passively listening.");
    find_next_expected_cc_tx();
}

void Manager::process_cc_tx_uid(const uint32_t& uid, const RXPacket& packet)
{
	for (uint8_t i=0; i<num_cc_txs; i++) {
		if (cc_txs[i].get_uid() == uid) {
			cc_txs[i].update(packet);
			break;
		}
	}
}

void Manager::run()
{
	if (num_cc_txs == 0) {
		poll_next_cc_trx();
	} else {
		if (millis() < (p_next_cc_tx->get_eta() - (CC_TX_WINDOW/2) )) {
			poll_next_cc_trx();
		} else  {
			wait_for_cc_tx();
		}
	}
}

void Manager::poll_next_cc_trx()
{
	// don't repeatedly poll iams; wait SAMPLE_PERIOD seconds;
	if (i_next_cc_trx==0) {
		if (millis() < timecode_polled_first_cc_trx+SAMPLE_PERIOD && retries==0) {
			return;
		} else {
			timecode_polled_first_cc_trx = millis();
		}
	}

	Serial.print(millis());
	Serial.print(" polling CC TRX ");
	Serial.println(id_next_cc_trx);
	rfm.poll_cc_trx(id_next_cc_trx);

	// wait for response
	const unsigned long start_time = millis();
	bool success = false;
	while (millis() < start_time+CC_TRX_TIMEOUT) {
		if (rfm.rx_packet_buffer.valid_data_is_available()
				&& process_rx_pack_buf_and_find_uid(id_next_cc_trx)) {
			success = true;
			break;
		}
	}

	if (success) {
		increment_i_of_next_cc_trx();
	} else {
		if (retries < MAX_RETRIES) {
			retries++;
		} else {
			increment_i_of_next_cc_trx();
		}
	}
}

void Manager::increment_i_of_next_cc_trx()
{
	i_next_cc_trx++;
	if (i_next_cc_trx >= num_cc_trxs) {
		i_next_cc_trx=0;
	}
	id_next_cc_trx = cc_trx_ids[i_next_cc_trx];

	retries = 0;
}

void Manager::wait_for_cc_tx()
{
	const unsigned long start_time = millis();

	// TODO handle roll-over over millis().

	// listen for WHOLE_HOUSE_TX for defined period.
	Serial.print(millis());
	Serial.println(" Window open!");
	bool success = false;
	while (millis() < (start_time+CC_TX_WINDOW) && !success) {
		if (rfm.rx_packet_buffer.valid_data_is_available() &&
				process_rx_pack_buf_and_find_uid(p_next_cc_tx->get_uid())) {
			success = true;
		}
	}

	Serial.print(millis());
	Serial.print(" window closed. success=");
	Serial.println(success);

	if (!success) {
		// tell whole-house TX it missed its slot
		p_next_cc_tx->missing();
	}

	find_next_expected_cc_tx();
}

const bool Manager::process_rx_pack_buf_and_find_uid(const uint32_t& target_uid)
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

				if (!uid_is_cc_trx(uid)) {
					process_cc_tx_uid(uid, *packet);
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

const bool Manager::uid_is_cc_trx(const uint32_t& uid) const
{
	for (uint8_t i=0; i<num_cc_trxs; i++) {
		if (cc_trx_ids[i] == uid) {
			return true;
		}
	}
	return false;
}

void Manager::find_next_expected_cc_tx()
{
	for (uint8_t i=0; i<num_cc_txs; i++) {
		if (cc_txs[i].get_eta() < p_next_cc_tx->get_eta()) {
			p_next_cc_tx = &cc_txs[i];
		}
	}
	Serial.print(millis());
	Serial.print(" Next expected tx has uid = ");
	Serial.print(p_next_cc_tx->get_uid());
	Serial.print(" eta=");
	Serial.println(p_next_cc_tx->get_eta());
}
