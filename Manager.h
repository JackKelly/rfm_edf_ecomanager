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

// TODO: not sure I need this Sensor class; just a WholeHouseTx class. Then I can
// just use a watts[3] array and make print() much prettier.
class Sensor {
public:
	Sensor();

	void update(const RXPacket& packet);

	void set_uid(const uint32_t& _uid);

	const unsigned long get_eta() const;

	void missing();

	const uint32_t get_uid() const;

protected:
	unsigned long eta; // estimated time of arrival in miliseconds since power-on
	uint32_t uid;
	uint16_t watts0;
};

// TODO: fine-tune the timings for each sensor (seems to differ between sensors)
// by learning from the data.
class WholeHouseTx : public Sensor {
public:
	WholeHouseTx();
	void update(const RXPacket& packet);

private:
	uint16_t watts1;
	uint16_t watts2;
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

	// Open the window EARLY_OPEN milliseconds before ETA.
	static const uint16_t EARLY_OPEN = 500;

	// length of time we're willing to wait
	// for the whole house tx
	static const uint16_t WINDOW = 1000;

	WholeHouseTx whole_house_txs[MAX_WHOLE_HOUSE_TXS];
	uint8_t num_whole_house_txs;
	uint8_t i_of_next_expected_tx;
	uint8_t next_iam;
	uint8_t num_iams;
	uint32_t iam_ids[MAX_NUM_IAMS];
	uint8_t retries;
	static const uint8_t MAX_RETRIES = 5; // for polling IAMs

	unsigned long timecode_polled_first_iam;

	Rfm12b rfm;

	/*
	 * Poll IAM with ID == iam_ids[next_iam]
	 * Listen for response.
	 */
	void poll_next_iam();

	void wait_for_whole_house_tx();

	void update_next_expected_tx();

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
