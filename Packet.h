#include <Arduino.h>

/**
 * Simple class for representing a packet of consecutive bytes.
 */
class Packet {
public:
	Packet(const uint8_t _packet_length = MAX_PACKET_LENGTH);

	void set_packet_length(const uint8_t _packet_length);

	/**
	 * Add a byte to the packet.
	 */
	volatile void add(const uint8_t value);
	volatile void add(const uint8_t* bytes, const uint8_t length);

	volatile const uint8_t get_next_byte();

	/**
	 * Print contents of packet to Serial port.
	 */
	void print() const;

	/**
	 * Returns true if we've reached the end of the packet.
	 */
	const bool done() const;

	/**
	 * Reset the byte_index to point to the first byte in this packet.
	 */
	volatile void reset();

	/**
	 * Assemble a packet from the following components (in order):
	 *   1. preamble
	 *   2. sync word
	 *   3. payload
	 *   4. (optional) checksum
	 *   5. tail
	 */
	volatile void assemble(const uint8_t payload[], const uint8_t payload_length,
			const bool add_checksum = false);

	/**
	 * @returns true if the checksum at the end of this packet matches
	 *          the checksum calculated from the payload.
	 */
	volatile const bool verify() const;

	/**
	 * @returns the wattage from an EDF IAM packet.
	 */
	volatile const uint16_t get_wattage() const;


private:
	volatile uint8_t packet_length; // number of bytes in this packet
	volatile uint8_t byte_index;    // index of next byte to write/read
	const static uint8_t MAX_PACKET_LENGTH = 22;
	// we can't use new() on the
	// arduino (not easily, anyway) so let's just have a statically declared
	// array of length MAX_PACKET_LENGTH.
	volatile uint8_t packet[MAX_PACKET_LENGTH];

	/**
	 * @returns the modular sum (the checksum algorithm used in the
	 *           EDF EcoManager protocol) given the payload.
	 */
	static const uint8_t modular_sum(volatile const uint8_t payload[], const uint8_t length);
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
	 * @returns true if packet is complete AFTER adding value to it.
	 */
	volatile const bool add(const uint8_t value);

	/**
	 * Print all packets to the Serial port and reset each packet.
	 */
	void print_and_reset();

	/**
	 * @return true if data is available.
	 */
	volatile const bool data_is_available() const;

private:
	const static uint8_t NUM_PACKETS = 5;
	uint8_t current_packet;
	Packet packets[NUM_PACKETS];
};
