#include <Arduino.h>

/**
 * Simple class for representing a packet of consecutive bytes
 * received from a Current Cost transmitter.
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
	void print() const;
	const bool full() const;
	volatile void reset();

private:
	volatile uint8_t packet_length; // number of bytes in a packet
	volatile uint8_t byte_index;    // index of next byte to write/read
	const static uint8_t MAX_PACKET_LENGTH = 22;
	// we can't use new() on the
	// arduino (not easily, anyway) so let's just have a statically declared
	// array of length MAX_PACKET_LENGTH.
	volatile uint8_t packet[MAX_PACKET_LENGTH];
};


/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */
class PacketBuffer {
public:
	// FIXME: concurrency issues. Research mutexes on Arduino.

	PacketBuffer(const uint8_t packet_length);

	/**
	 * @returns true if packet is complete AFTER adding value to it.
	 */
	volatile const bool add(const uint8_t value);
	void print_and_reset();
	volatile const bool data_is_available() const;

private:
	const static uint8_t NUM_PACKETS = 5;
	uint8_t current_packet;
	Packet packets[NUM_PACKETS];
};
