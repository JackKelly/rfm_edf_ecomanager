/*
 *      Author: Jack Kelly
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE
 * LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
 * PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE
 * DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include "consts.h"

/**
 * Simple base class for representing a packet of consecutive bytes.
 */
class Packet {
public:
	Packet();
	virtual ~Packet();

	void set_packet_length(const index_t& _packet_length);

	void append(const byte& value);

	void append(const byte bytes[], const index_t& length);

	/**
	 * Print contents of packet to Serial port.
	 * TODO: is this still needed?
	 */
	void print_bytes() const;

	/**
	 * Reset the byte_index to point to the first byte in this packet.
	 */
	virtual void reset();

	/**
	 * Returns true if we've reached the end of the packet.
	 * TODO: can this be made private?
	 */
	const bool done() const;

	// TODO: can this be made private?
	const volatile index_t& get_byte_index() const;

#ifdef TESTING
	const volatile index_t& get_length() const;
	const volatile byte* get_packet() const;
#endif

protected:
	/* Longest packet = 1B preamble + 2B sync + 12B EDF IAM + 2B tail
	 *  If you want to mimick CC_TX packets then this needs to be set to 21!  */
	const static index_t MAX_PACKET_LENGTH = 17;

	/****************************************************
	 * Member variables used within ISR and outside ISR *
	 ****************************************************/
	volatile index_t length; // number of bytes in this packet
	volatile index_t byte_index;    // index of next byte to write/read
	// we can't use new() on the
	// arduino (not easily, anyway) so let's just have a statically declared
	// array of length MAX_PACKET_LENGTH.
	volatile byte packet[MAX_PACKET_LENGTH];

	/********************************************
	 * Private methods                          *
	 ********************************************/
	/**
	 * @returns the modular sum (the checksum algorithm used in the
	 *           EDF EcoManager protocol) given the payload.
	 */
	static const byte modular_sum(
			const volatile byte payload[],
			const index_t& length);

};

class RXPacket : public Packet
{
public:
	RXPacket();

	void print_id_and_watts() const;

	void append(const byte& value); // override

	const bool is_ok();

	const bool is_pairing_request() const;

    const volatile TxType& get_tx_type() const;

	const id_t& get_id() const;

	const watts_t* get_watts() const;

	void reset();

	volatile const millis_t& get_timecode() const;

private:
	/********************
	 * Consts           *
	 * ******************/
	const static index_t CC_TRX_PACKET_LENGTH = 12;
	const static index_t CC_TX_PACKET_LENGTH  = 16;

	/****************************************************
	 * Member variables used within ISR and outside ISR *
	 ****************************************************/
	volatile TxType tx_type; // is this packet from a transmit-only sensor (as opposed to a transceiver)?
	volatile millis_t timecode;

	/******************************************
	 * Member variables never used within ISR *
	 ******************************************/
	enum Health {NOT_CHECKED, OK, BAD} health; // does the checksum or de-manchesterisation check out?
	watts_t watts[3]; // the decoded reading from sensors
	id_t id; // the sensor radio ID

	/********************************************
	 * Private methods                          *
	 ********************************************/

    /**
     * Run this after packet has been received fully to
     * demanchesterise (if from TX), set health, watts and id.
     */
    void post_process();

	/**
	 * @ return true if checksum in packet matches calculated checksum
	 */
	const Health verify_checksum() const;

	/**
	 * Decodes watts and sets Packet::watts
	 */
	void decode_wattage();

	void decode_id();

	/**
	 * De-Manchesterise this packet.
	 *
	 * Data from CC TXs appears to be "Manchesterised" such that x bytes of
	 * Manchesterised data is decoded to produce x/2 bytes of data.
	 * A bit pair of 01 in the source encodes a 0 and a 10 encodes a 1.
	 * The fact that CC TX data is Manchesterised appears to have been
	 * first figured out by gangliontwitch.
	 *
	 * @return OK if de-manchesterisation went OK
	 * @return BAD if any illegal bit pairs (11 or 00) were found
	 */
	const Health de_manchesterise();

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
	void assemble(const byte payload[], const index_t& payload_length,
			const bool add_checksum = false);

	const byte get_next_byte();

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
	const bool append(const byte& value);

	const static index_t NUM_PACKETS = 5;
	index_t current_packet;
	RXPacket packets[NUM_PACKETS];
};

#endif /* PACKET_H_ */
