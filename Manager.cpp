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
#include "Logger.h"
#include "utils.h"

Manager::Manager()
: auto_pair(true), pair_with(ID_INVALID), print_packets(ALL_VALID),
  retries(0), timecode_polled_first_cc_trx(0)
{}


void Manager::init()
{
    rfm.init();
    rfm.enable_rx();
}


void Manager::run()
{
    //************* HANDLE TRANSMITTERS AND TRANSCEIVERS ***********
    if (cc_txs.get_size() == 0) {
        // There are no CC TXs so all we have to do is poll TRXs
        poll_next_cc_trx();
    } else {
        if (millis() < (cc_txs.current().get_eta() - (CC_TX_WINDOW/2) )) {
            // We're far enough away from the next expected CC TX transmission
            // to mean that we have time to poll TRXs
            poll_next_cc_trx();
        } else  {
            wait_for_cc_tx();
        }
    }

    // Make sure we always check for RX packets
    process_rx_pack_buf_and_find_id(0);

    if (Serial.available()) {
        handle_serial_commands();
    }
}

void Manager::handle_serial_commands()
{
    char incomming_byte = Serial.read();
    switch (incomming_byte) {
    case 'a': auto_pair = true;  Serial.println("ACK auto_pair mode on"); break;
    case 'm': auto_pair = false; Serial.println("ACK audo_pair mode off"); break;
    case 'p':
        if (auto_pair) {
            Serial.println("NAK Please enable manual pairing mode ('m') before issuing 'p' command.");
        } else {
            Serial.println("ACK Please enter ID followed by carriage return:");
            pair_with = utils::read_uint32_from_serial();
            Serial.print("ACK pair_with set to ");
            Serial.println(pair_with);
        }
        break;
    case 'v':
        Serial.println("ACK enter log level:");
        print_log_levels();
        Logger::log_threshold = (Level)utils::read_uint32_from_serial();
        Serial.print("ACK Log level set to ");
        print_log_level(Logger::log_threshold);
        Serial.println("");
        break;
    case 'k': print_packets = ONLY_KNOWN; Serial.println("ACK only print data from known transmitters"); break;
    case 'u': print_packets = ALL_VALID; Serial.println("ACK print all valid packets"); break;
    case 'b': print_packets = ALL; Serial.println("ACK print all"); break;
    case 'n': cc_txs.get_id_from_serial();  break;
    case 'N': cc_trxs.get_id_from_serial(); break;
    case '\r': break; // ignore carriage returns
    default:
        Serial.print("NAK unrecognised command '");
        Serial.print(incomming_byte);
        Serial.println("'");
        break;
    }
}


void Manager::poll_next_cc_trx()
{
    if (cc_trxs.get_size() == 0) return;

	// don't continually poll TRXs;
    // instead wait SAMPLE_PERIOD between polling the first TRX and polling it again
	if (cc_trxs.get_i()==0) {
		if (millis() < timecode_polled_first_cc_trx+SAMPLE_PERIOD && retries==0) {
			return; // We've finished polling all TRXs for this SAMPLE_PERIOD.
		} else {
			timecode_polled_first_cc_trx = millis();
		}
	}

	rfm.poll_cc_trx(cc_trxs.current().id);
	const bool success = wait_for_response(cc_trxs.current().id, CC_TRX_TIMEOUT);

	if (success) {
        // We got a reply from the TRX we polled
		cc_trxs.next();
		retries = 0;
	} else {
	    // We didn't get a reply from the TRX we polled
		if (retries < MAX_RETRIES) {
            log(DEBUG, "No response from TRX %lu, retries=%d. Retrying...", cc_trxs.current().id, retries);
			retries++;
		} else {
			cc_trxs.next();
			log(INFO, "No response from TRX %lu after retrying. Giving up.", cc_trxs.current().id);
			retries = 0;
		}
	}
}


void Manager::wait_for_cc_tx()
{
    // TODO handle roll-over over millis().

    // listen for TX for defined period.
    log(DEBUG, "Window open! Expecting %lu at %lu", cc_txs.current().id, cc_txs.current().get_eta());
    bool success = wait_for_response(cc_txs.current().id, CC_TX_WINDOW);
    log(DEBUG, "Window closed. success=%d", success);

    if (!success) {
        // tell whole-house TX it missed its slot
        cc_txs.current().missing();
    }
}


const bool Manager::wait_for_response(const id_t& id, const millis_t& wait_duration)
{
    // wait for response
    const millis_t start_time = millis();
    const millis_t end_time   = start_time + wait_duration;
    bool success = false;

    while (millis() < end_time) {
        if (process_rx_pack_buf_and_find_id(id)) {
            // We got a reply from the TRX we polled
            success = true;
            break;
        }
    }
    return success;
}


const bool Manager::process_rx_pack_buf_and_find_id(const uint32_t& target_id)
{
	bool success = false;
	uint32_t id;
	RXPacket* packet = NULL; // just using this pointer to make code more readable

	/* Loop through every packet in packet buffer. If it's done then post-process it
	 * and then check if it's valid.  If so then handle the different types of
	 * packet.  Finally reset the packet and return.
	 */
	for (uint8_t packet_i=0; packet_i<rfm.rx_packet_buffer.NUM_PACKETS; packet_i++) {

		packet = &rfm.rx_packet_buffer.packets[packet_i];
		if (packet->done()) {
		    packet->post_process();
			if (packet->is_ok()) {
				id = packet->get_id();
				success = (id == target_id); // Was this the packet we were looking for?

				//******** PAIRING REQUEST **********************
				if (packet->is_pairing_request()) {
				    if (packet->is_cc_tx() && cc_txs.find(id)) {
				        // ignore pair request from CC_TX we're already paired with
				    } else if (!packet->is_cc_tx() && cc_trxs.find(id)) {
				        // ignore pair request from CC_TRX we're already paired with
				    } else if (auto_pair) {
				        // Auto pair mode. Go ahead and pair.
				        pair_with = id;
				        pair(packet->is_cc_tx());
                    } else if (pair_with == id) {
                        // Manual pair mode and pair_with has already been set so pair.
                        pair(packet->is_cc_tx());
			        } else {
			            // Manual pair mode. Tell user about pair request.
			            Serial.print("{PR: ");
			            Serial.print(id);
			            Serial.println("}");
				    }
				}
				//********* CC TX (transmit-only sensor) ********
				else if (packet->is_cc_tx()) {
				    bool found;
				    index_t cc_tx_i;
				    found = cc_txs.find(id, &cc_tx_i);
				    if (found) { // received ID is a CC_TX id we know about
				        cc_txs[cc_tx_i].update(*packet);
				        packet->print_id_and_watts(); // send data over serial
	                    cc_txs.next();
				    } else {
	                    log(INFO, "Rx'd CC_TX packet with unknown ID %lu", id);
	                    if (print_packets >= ALL_VALID) {
	                        packet->print_id_and_watts(); // send data over serial
	                    }
				    }
				}
				//****** CC TRX (transceiver; e.g. EDF IAM) ******
				else if (cc_trxs.find(id)) {
				    // Received ID is a CC_TRX id we know about
                    packet->print_id_and_watts(); // send data over serial
				}
				//********* UNKNOWN TRX ID *************************
				else {
                    log(INFO, "Rx'd CC_TRX packet with unknown ID %lu", id);
                    if (print_packets >= ALL_VALID) {
                        packet->print_id_and_watts(); // send data over serial
                    }
				}

			} else {
				log(INFO, "Rx'd broken packet");
				if (print_packets == ALL) {
				    packet->print_bytes();
				}
			}
	        packet->reset();
		}
	}

	return success;
}


void Manager::pair(const bool is_cc_tx)
{
    bool success = false;

    if (is_cc_tx) {
        success = cc_txs.append(pair_with);
    } else { // transceiver. So we need to ACK.
        rfm.ack_cc_trx(pair_with);
        if ( wait_for_response(pair_with, CC_TRX_TIMEOUT) ) {
            // Only append if we get a response.
            // If we don't get a response then we'll try to pair again
            // when the TRX next sends a pair request.
            success = cc_trxs.append(pair_with);
        }
    }

    if (success) {
        Serial.print("{pw: ");
        Serial.print(pair_with);
        Serial.println(" }");
        pair_with = ID_INVALID;
    }
}
