#include <Arduino.h>

/**
 * Simple class for representing a packet of consecutive bytes.
 */
class Packet {
public:
	Packet(const uint8_t _packet_length = MAX_PACKET_LENGTH);

	void set_packet_length(const uint8_t _packet_length);

	void add(const uint8_t* bytes, const uint8_t length);
	/**
	 * Print contents of packet to Serial port.
	 */
	void print() const;


	/**
	 * Reset the byte_index to point to the first byte in this packet.
	 */
	void reset();

	/**
	 * Assemble a packet from the following components (in order):
	 *   1. preamble
	 *   2. sync word
	 *   3. payload
	 *   4. (optional) checksum
	 *   5. tail
	 */
	void assemble(const uint8_t payload[], const uint8_t payload_length,
			const bool add_checksum = false);


	/**
	 * Run this one packet has been received fully.
	 */
	void post_process();


	/****************************************
	 * FUNCTIONS WHICH MAY BE CALLED FROM AN
	 * INTERRUPT HANDLER
	 * and hence must be declared volatile
	 * **************************************/

	/**
	 * Add a byte to the packet.
	 */
	volatile void add(const uint8_t value);

	/**
	 * Returns true if we've reached the end of the packet.
	 */
	volatile const bool done() const;

	volatile const uint8_t get_next_byte();

private:
	volatile uint8_t packet_length; // number of bytes in this packet
	volatile uint8_t byte_index;    // index of next byte to write/read
	const static uint8_t EDF_IAM_PACKET_LENGTH = 12;
	const static uint8_t WHOLE_HOUSE_TX_PACKET_LENGTH = 16;
	const static uint8_t MAX_PACKET_LENGTH = 22;
	// we can't use new() on the
	// arduino (not easily, anyway) so let's just have a statically declared
	// array of length MAX_PACKET_LENGTH.
	volatile uint8_t packet[MAX_PACKET_LENGTH];
	volatile bool whole_house_tx;
	bool packet_ok; // does the checksum / de-manchesterisation check out?
	const static uint16_t WATTS_NOT_VALID = 0xFFFF;
	uint16_t watts[3];
	const static uint32_t UID_NOT_VALID = 0xFFFFFFFF;
	uint32_t uid;

	/**
	 * @returns the modular sum (the checksum algorithm used in the
	 *           EDF EcoManager protocol) given the payload.
	 */
	static const uint8_t modular_sum(volatile const uint8_t payload[], const uint8_t length);

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


/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */
class PacketBuffer {
public:

	PacketBuffer(const uint8_t packet_length);

	/**
	 * Print all packets to the Serial port and reset each packet.
	 */
	void print_and_reset();

	/**
	 * @return true if data is available.
	 */
	const bool data_is_available() const;

	/****************************************
	 * FUNCTIONS WHICH MAY BE CALLED FROM AN
	 * INTERRUPT HANDLER
	 * and hence must be declared volatile
	 * **************************************/

	/**
	 * @returns true if packet is complete AFTER adding value to it.
	 */
	volatile const bool add(const uint8_t value);


private:
	const static uint8_t NUM_PACKETS = 5;
	uint8_t current_packet;
	Packet packets[NUM_PACKETS];
};
