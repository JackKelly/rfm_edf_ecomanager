#include "Packet.h"

/**
 * Simple class for representing a packet of consecutive bytes
 * received from a Current Cost transmitter.
 */

Packet::Packet(): bytes_read(0) {}

/**
 * Add a byte to the packet.
 */
volatile void Packet::add(const uint8_t value) {
	if (bytes_read < PACKET_SIZE) {
		packet[bytes_read++] = value;
	}
}

void Packet::print() const {
	for (int i=0; i<PACKET_SIZE; i++) {
		Serial.print(packet[i], HEX);
		Serial.print(" ");
	}
	Serial.print("\r\n");
}

bool Packet::full() {
	return bytes_read == PACKET_SIZE;
}

volatile void Packet::reset() {
	bytes_read = 0;
}

/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */

// FIXME: concurrency issues. Research mutexes on Arduino.

PacketBuffer::PacketBuffer(): current_packet(0) {}

/**
 * @returns true if packet is complete AFTER adding value to it.
 */
volatile const bool PacketBuffer::add(const uint8_t value) {
	packets[current_packet].add(value);

	if (packets[current_packet].full()) {
		if (current_packet >= NUM_PACKET) {
			Serial.println("NO MORE BUFFERS!");
		} else {
			current_packet++;
		}
		return true;
	} else {
		return false;
	}
}

void PacketBuffer::print_and_reset() {
	for (int i=0; i<current_packet; i++) {
		packets[i].print();
		packets[i].reset();
	}
	if (packets[current_packet].full()) {
		packets[current_packet].print();
		packets[current_packet].reset();
	}
	current_packet = 0;
}

volatile const bool PacketBuffer::data_is_available() const {
	return current_packet > 0;
}

