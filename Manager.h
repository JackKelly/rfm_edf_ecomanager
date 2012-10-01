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

// TODO: tweak SAMPLE_PERIOD per Sensor based on experience (each transmitter
// seems to have a slightly different transmit period).
class Sensor {
public:
	Sensor();
	void update(const RXPacket& packet);
	void missing();
	void set_uid(const uint32_t& _uid);
	const uint32_t get_uid() const;
	const unsigned long get_eta() const;

protected:
	unsigned long eta; // estimated time of arrival in miliseconds since power-on
	uint32_t uid;
};


class Manager {
public:
	Manager();
	void init();
	void run();
private:
	static const uint8_t MAX_WHOLE_HOUSE_TXS = 5;

	//**************************
	// Timings (in milliseconds)

	// length of time we're willing to wait
	// for the whole house tx.  We'll open the window
	// half of WINDOW before ETA.
	static const uint16_t WINDOW = 1000;

	static const unsigned long IAM_TIMEOUT = 100; // milliseconds to wait for reply

	Sensor whole_house_txs[MAX_WHOLE_HOUSE_TXS];
	uint8_t num_whole_house_txs;
	uint8_t i_of_next_expected_tx;
	uint8_t next_iam;
	uint8_t num_iams;
	uint32_t iam_ids[MAX_NUM_IAMS];
	uint8_t retries;
	static const uint8_t MAX_RETRIES = 5; // for polling IAMs

	unsigned long timecode_polled_first_iam;

	Rfm12b rfm;

	/***************************
	 * Private methods
	 ***************************/

	/*
	 * Poll IAM with ID == iam_ids[next_iam]
	 * Listen for response.
	 */
	void poll_next_iam();

	void wait_for_whole_house_tx();

	void find_next_expected_tx();

	void process_whole_house_uid(const uint32_t& uid, const RXPacket& packet);

	const bool uid_is_iam(const uint32_t& uid) const;

	void increment_next_iam();

	/**
	 * Process every packet in rx_packet_buffer appropriately
	 *
	 * @return true if a packet corresponding to uid is found
	 */
	const bool process_rx_packet_buffer(const uint32_t& uid);
};

#endif /* MANAGER_H_ */
