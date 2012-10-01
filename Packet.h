#ifndef PACKET_H_
#define PACKET_H_

#include <Arduino.h>

/**
 * Simple base class for representing a packet of consecutive bytes.
 */
class Packet {
public:
	Packet();

	void set_packet_length(const uint8_t& _packet_length);

	void append(const uint8_t& value);

	void append(const uint8_t* bytes, const uint8_t& length);

	/**
	 * Print contents of packet to Serial port.
	 */
	void print_bytes() const;

	/**
	 * Reset the byte_index to point to the first byte in this packet.
	 * Also reset all other contents
	 */
	void reset();

	/**
	 * Returns true if we've reached the end of the packet.
	 */
	const bool done() const;

	const uint8_t get_byte_index() const;

protected:
	const static uint8_t MAX_PACKET_LENGTH = 22;

	/****************************************************
	 * Member variables used within ISR and outside ISR *
	 ****************************************************/
	volatile uint8_t packet_length; // number of bytes in this packet
	volatile uint8_t byte_index;    // index of next byte to write/read
	// we can't use new() on the
	// arduino (not easily, anyway) so let's just have a statically declared
	// array of length MAX_PACKET_LENGTH.
	volatile uint8_t packet[MAX_PACKET_LENGTH];

	/********************************************
	 * Private methods                          *
	 ********************************************/
	/**
	 * @returns the modular sum (the checksum algorithm used in the
	 *           EDF EcoManager protocol) given the payload.
	 */
	static const uint8_t modular_sum(
			const volatile uint8_t payload[],
			const uint8_t& length);

};

class RXPacket : public Packet
{
public:
	RXPacket();

	void print_uid_and_watts() const;

	void append(const uint8_t& value); // override

	/**
	 * Run this one packet has been received fully.
	 */
	void post_process();

	/*
	 * @return contents of packet_ok
	 */
	const bool is_ok() const;

	const uint32_t& get_uid() const;

	const uint16_t* get_watts() const;

	volatile const unsigned long& get_timecode() const;

	void reset();

private:
	/********************
	 * Consts           *
	 * ******************/
	const static uint8_t EDF_IAM_PACKET_LENGTH = 12;
	const static uint8_t WHOLE_HOUSE_TX_PACKET_LENGTH = 16;

	/****************************************************
	 * Member variables used within ISR and outside ISR *
	 ****************************************************/
	volatile bool whole_house_tx; // is this packet from a whole-house tx?
	volatile unsigned long timecode;

	/******************************************
	 * Member variables never used within ISR *
	 ******************************************/
	bool packet_ok; // does the checksum or de-manchesterisation check out?
	uint16_t watts[3]; // the decoded reading from sensors
	uint32_t uid; // the sensor radio ID

	/********************************************
	 * Private methods                          *
	 ********************************************/
	/**
	 * @ return true if checksum in packet matches calculated checksum
	 */
	const bool verify_checksum() const;

	/**
	 * sets watts
	 */
	void decode_wattage();

	void decode_uid();

	/**
	 * DeManchesterise this packet
	 * @return true if de-manchesterisation went OK.
	 */
	const bool de_manchesterise();

};

class TXPacket : public Packet
{
public:

	/**
	 * Assemble a packet from the following components (in order):
	 *   1. preamble
	 *   2. sync word
	 *   3. payload
	 *   4. (optional) checksum
	 *   5. tail
	 */
	void assemble(const uint8_t payload[], const uint8_t& payload_length,
			const bool add_checksum = false);

	const uint8_t get_next_byte();

private:
	/********************************************
	 * Private methods                          *
	 ********************************************/

};


/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */
class PacketBuffer {
public:

	PacketBuffer(const uint8_t& packet_length);

	/**
	 * Print all packets to the Serial port and reset each packet.
	 */
	void print_and_reset();

	/**
	 * @return true if data is available.
	 */
	const bool data_is_available() const;

	const bool valid_data_is_available();

	void reset_all();

	/****************************************
	 * FUNCTIONS WHICH MAY BE CALLED FROM AN
	 * INTERRUPT HANDLER
	 * **************************************/

	/**
	 * @returns true if packet is complete AFTER adding value to it.
	 */
	const bool add(const uint8_t& value);

	const static uint8_t NUM_PACKETS = 5;
	uint8_t current_packet;
	RXPacket packets[NUM_PACKETS];
};

#endif /* PACKET_H_ */
