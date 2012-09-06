#include <Arduino.h>

/**
 * Simple class for representing a packet of consecutive bytes
 * received from a Current Cost transmitter.
 */
class Packet {
public:
	Packet();

	/**
	 * Add a byte to the packet.
	 */
	volatile void add(const uint8_t value);
	void print() const;
	bool full();
	volatile void reset();

private:
	static const uint8_t PACKET_SIZE = 12; // number of bytes in a packet
	volatile uint8_t bytes_read; // number of bytes read so far
	volatile uint8_t packet[PACKET_SIZE];
};

/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */
class PacketBuffer {
public:
	// FIXME: concurrency issues. Research mutexes on Arduino.

	PacketBuffer();

	/**
	 * @returns true if packet is complete AFTER adding value to it.
	 */
	volatile const bool add(const uint8_t value);
	void print_and_reset();
	volatile const bool data_is_available() const;

private:
	static const uint8_t NUM_PACKET = 5;
	uint8_t current_packet;
	Packet packets[NUM_PACKET];
};
