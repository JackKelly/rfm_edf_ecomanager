/*
 * Manager.h
 *
 *  Created on: 26 Sep 2012
 *      Author: jack
 */

#ifndef MANAGER_H_
#define MANAGER_H_

#include <Arduino.h>
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
    uint32_t pair_with; /* radio ID to pair with */

    enum {ONLY_KNOWN, ALL_VALID, ALL} print_packets;

	/*****************************************
	 * CC TX (e.g. whole-house transmitters) *
	 *****************************************/
    static const uint8_t MAX_CC_TXS = 5;
    CcTx cc_tx_array[MAX_CC_TXS];
    CcTxArray cc_txs;

	/* length of time we're willing to wait
	 * for a CC TX.  We'll open the window
	 * half of WINDOW before CC TX's ETA. */
	static const uint16_t CC_TX_WINDOW = 1000;

    /*****************************************
     * CC TRX (e.g. EDF IAMs)                *
     *****************************************/
    static const uint8_t  MAX_CC_TRXS = 35;   /* number of CC TRXs (e.g. EDF IAMs) */
    CcTrx cc_trx_array[MAX_CC_TRXS];
    CcTrxArray cc_trxs;

    static const uint32_t CC_TRX_TIMEOUT = 100; // milliseconds to wait for reply

	uint8_t retries; // for polling CC TRXs
	static const uint8_t MAX_RETRIES = 5; // for polling CC TRXs

	uint32_t timecode_polled_first_cc_trx;

	/***************************
	 * Private methods
	 ***************************/

	/* Poll CC TRX (e.g. EDF IAM) with ID == id_next_cc_trx
	 * Listen for response. */
	void poll_next_cc_trx();

	void wait_for_cc_tx();

	/**
	 * Process every packet in rx_packet_buffer appropriately
	 *
	 * @return true if a packet corresponding to id is found
	 */
	const bool process_rx_pack_buf_and_find_id(const uint32_t& id);

	/**
	 * If pair_with != ID_INVALID then pair with pair_with.
	 */
	void pair(const bool is_cc_tx);

};

#endif /* MANAGER_H_ */
