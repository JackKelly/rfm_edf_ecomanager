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
#include "debug.h"

Manager::Manager()
: p_next_cc_tx(cc_txs), i_next_cc_trx(0), retries(0), timecode_polled_first_cc_trx(0)
{
	// TODO: this stuff needs to be programmed over serial not hard-coded.
	num_cc_txs = 2;
	cc_txs[0].set_id(895);
	cc_txs[1].set_id(28);

	num_cc_trxs = 1;
	cc_trx_ids[0] = 0x55100003;
	id_next_cc_trx = cc_trx_ids[0];
}


void Manager::init()
{
    rfm.init();
    rfm.enable_rx();
    find_next_expected_cc_tx();
}


void Manager::run()
{
    if (num_cc_txs == 0) {
        // There are no CC TXs so all we have to do it poll TRXs
        poll_next_cc_trx();
    } else {
        if (millis() < (p_next_cc_tx->get_eta() - (CC_TX_WINDOW/2) )) {
            // We're far enough away from the next expected CC TX transmission
            // to mean that we have time to poll TRXs
            poll_next_cc_trx();
        } else  {
            wait_for_cc_tx();
        }
    }
}


void Manager::poll_next_cc_trx()
{
	// don't continually poll TRXs;
    // instead wait SAMPLE_PERIOD between polling the first TRX.
	if (i_next_cc_trx==0) {
		if (millis() < timecode_polled_first_cc_trx+SAMPLE_PERIOD && retries==0) {
			return;
		} else {
			timecode_polled_first_cc_trx = millis();
		}
	}

	rfm.poll_cc_trx(id_next_cc_trx);

	// wait for response
	const uint32_t start_time = millis();
	bool success = false;
	while (millis() < start_time+CC_TRX_TIMEOUT) {
		if (rfm.rx_packet_buffer.valid_data_is_available()
				&& process_rx_pack_buf_and_find_id(id_next_cc_trx)) {
	        // We got a reply from the TRX we polled
			success = true;
			break;
		}
	}

	if (success) {
        // We got a reply from the TRX we polled
		increment_i_of_next_cc_trx();
	} else {
	    // We didn't get a reply from the TRX we polled
		if (retries < MAX_RETRIES) {
			retries++;
		} else {
			increment_i_of_next_cc_trx();
		}
	}
}


void Manager::wait_for_cc_tx()
{
	const uint32_t start_time = millis();

	// TODO handle roll-over over millis().

	// listen for WHOLE_HOUSE_TX for defined period.
	debug(INFO, "Window open! Expecting %lu at %lu", p_next_cc_tx->get_id(), p_next_cc_tx->get_eta());
	bool success = false;
	while (millis() < (start_time+CC_TX_WINDOW) && !success) {
		if (rfm.rx_packet_buffer.valid_data_is_available() &&
				process_rx_pack_buf_and_find_id(p_next_cc_tx->get_id())) {
			success = true;
		}
	}

	debug(INFO, "Window closed. success=%d", success);

	if (!success) {
		// tell whole-house TX it missed its slot
		p_next_cc_tx->missing();
	}

	find_next_expected_cc_tx();
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


Sensor* Manager::find_cc_tx(const uint32_t& id)
{
    for (uint8_t i=0; i<num_cc_txs; i++) {
        if (cc_txs[i].get_id() == id) {
            return &cc_txs[i];
        }
    }
    return NULL;
}


const bool Manager::process_rx_pack_buf_and_find_id(const uint32_t& target_id)
{
	bool success = false;
	uint32_t id;
	RXPacket* packet = NULL;
	Sensor* cc_tx = NULL;

	for (uint8_t packet_i=0; packet_i<=rfm.rx_packet_buffer.current_packet; packet_i++) {
		packet = &rfm.rx_packet_buffer.packets[packet_i];
		if (packet->done()) {
			if (packet->is_ok()) {
				id = packet->get_id();
				success = (id == target_id); // Was this the packet we were looking for?

				if (packet->is_pairing_request()) {
				    Serial.print("{PR: ");
				    Serial.print(id);
				    Serial.println("}");
				} else if (packet->is_cc_tx()) {
				    cc_tx = find_cc_tx(id);
				    if (cc_tx) { // Is received ID is a CC_TX id we know about?
				        cc_tx->update(*packet);
				        packet->print_id_and_watts(); // send data over serial
				    }
				} else if (id_is_cc_trx(id)) {
				    // Received ID is a CC_TRX id we know about
				    // TODO don't transmit both packets?
				    packet->print_id_and_watts(); // send data over serial
				} else { // TODO: handle pair requests and EDF IAM manual mode changes
	                debug(INFO, "Unknown ID: ");
	                packet->print_id_and_watts(); // send data over serial
				}

			} else {
				debug(INFO, "Broken packet received.");
			}
		}
	}

	rfm.rx_packet_buffer.reset_all();
	return success;
}


const bool Manager::id_is_cc_trx(const uint32_t& id) const
{
	for (uint8_t i=0; i<num_cc_trxs; i++) {
		if (cc_trx_ids[i] == id) {
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
	debug(INFO, "Next expected CC_TX has ID=%lu, ETA=%lu", p_next_cc_tx->get_id(), p_next_cc_tx->get_eta());
}
