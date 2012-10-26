/*
 * Manager.h
 *
 *  Created on: 26 Sep 2012
 *      Author: jack
 *
 *  This class runs the show.  It is responsible for:
 *
 *    - Polling TRXs in sequence (a "roll call")
 *       - retrying MAX_RETRIES times if no response is received
 *       - ensure we only do one roll call per SAMPLE_PERIOD
 *
 *    - Listening for TXs.
 *       - We try to learn when TXs are expected to arrive so we can pause
 *         TRX polling for CC_TX_WINDOW milliseconds to minimise
 *         the chances of an RF collision.
 *
 *    - Listening for commands from the serial port.
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE
 * LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
 * PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE
 * DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */

#ifndef MANAGER_H_
#define MANAGER_H_

//#include <Arduino.h>
#include "consts.h"
#include "Rfm12b.h"
#include "Packet.h"
#include "CcTx.h"

class Manager {
public:
	Manager();
	void init();
	void run();
private:
    Rfm12b rfm;

    bool auto_pair; /* auto_pair mode on or off? */
    id_t pair_with; /* radio ID to pair with */

    enum {
        ONLY_KNOWN, /* Only print packets we know about */
        ALL_VALID,  /* Print all valid packets */
        ALL         /* Print all packets, including broken ones */
    } print_packets;

	/*****************************************
	 * CC TX (e.g. whole-house transmitters) *
	 *****************************************/
    CcTxArray cc_txs;

    /*****************************************
     * CC TRX (e.g. EDF IAMs)                *
     *****************************************/
	CcTrxArray cc_trxs;

	uint8_t retries; /* number of times we've tried to poll the current TRX */

	/* We need to keep track of when we started the TRX roll call so we can
	 * ensure that we only do one roll call per SAMPLE_PERIOD */
	millis_t time_to_start_next_trx_roll_call;

	/***************************
	 * Private methods
	 ***************************/

	/* Poll CC TRX (e.g. EDF IAM) with ID == id_next_cc_trx
	 * Listen for response. */
	void poll_next_cc_trx();

	void wait_for_cc_tx();

	/* @return true if we get a response from id before wait_duration is up */
	bool wait_for_response(const id_t& id, const millis_t& wait_duration);

	void handle_serial_commands();

	/**
	 * Process every packet in rx_packet_buffer appropriately
	 *
	 * @return true if a packet corresponding to id is found
	 */
	bool process_rx_pack_buf_and_find_id(const id_t& id);

	void handle_pair_request(const TxType& tx_type, const id_t& id);

	/**
	 * If pair_with != ID_INVALID then pair with pair_with.
	 */
	void pair(const TxType& tx_type);

};

#endif /* MANAGER_H_ */
