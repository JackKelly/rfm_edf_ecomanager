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
	 */
	void reset();

	/**
	 * Returns true if we've reached the end of the packet.
	 */
	const bool done() const;

	const uint8_t get_byte_index() const;

protected:
	/* Longest packet = 1B preamble + 2B sync + 12B EDF IAM + 2B tail
	 *  If you want to mimick CC_TX packets then this needs to be set to 21!  */
	const static uint8_t MAX_PACKET_LENGTH = 17;

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

	void print_id_and_watts() const;

	void append(const uint8_t& value); // override

	/**
	 * Run this after packet has been received fully to
	 * set packet_ok, watts and id.
	 */
	void post_process();

	/*
	 * @return contents of packet_ok
	 */
	const bool is_ok() const;

	const bool is_pairing_request() const;

    const bool is_cc_tx() const;

	const uint32_t& get_id() const;

	const uint16_t* get_watts() const;

	volatile const unsigned long& get_timecode() const;

private:
	/********************
	 * Consts           *
	 * ******************/
	const static uint8_t CC_TRX_PACKET_LENGTH = 12;
	const static uint8_t CC_TX_PACKET_LENGTH  = 16;

	/****************************************************
	 * Member variables used within ISR and outside ISR *
	 ****************************************************/
	volatile bool cc_tx; // is this packet from a transmit-only sensor (as opposed to a transceiver)?
	volatile unsigned long timecode;

	/******************************************
	 * Member variables never used within ISR *
	 ******************************************/
	bool packet_ok; // does the checksum or de-manchesterisation check out?
	uint16_t watts[3]; // the decoded reading from sensors
	uint32_t id; // the sensor radio ID

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

	void decode_id();

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

};


/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */
class PacketBuffer {
public:

	PacketBuffer();

	/****************************************
	 * FUNCTIONS WHICH MAY BE CALLED FROM AN
	 * INTERRUPT HANDLER
	 * **************************************/

	/**
	 * @returns true if packet is complete AFTER appending value to it.
	 */
	const bool append(const uint8_t& value);

	const static uint8_t NUM_PACKETS = 5;
	uint8_t current_packet;
	RXPacket packets[NUM_PACKETS];
};

#endif /* PACKET_H_ */
