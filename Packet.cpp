#include "Packet.h"

/**
 * Simple class for representing a packet of consecutive bytes
 * received from a Current Cost transmitter.
 */

Packet::Packet(const uint8_t _packet_length)
: packet_length(_packet_length), byte_index(0)
{
	if (packet_length > MAX_PACKET_LENGTH) {
		Serial.println("ERROR: packet_length > MAX_PACKET_LENGTH!");
	}
}

void Packet::set_packet_length(const uint8_t _packet_length)
{
	packet_length = _packet_length;
}

/**
 * Add a byte to the packet.
 */
volatile void Packet::add(const uint8_t value)
{
	if (!full()) {
		packet[byte_index++] = value;
	}
}

volatile void Packet::add(const uint8_t* bytes, const uint8_t length)
{
	for (int i=0; i<length; i++) {
		add(bytes[i]);
	}
}

volatile const uint8_t Packet::get_next_byte()
{
	if (!full()) {
		return packet[byte_index++];
	}
	return 0;
}

void Packet::print() const {
	for (int i=0; i<packet_length; i++) {
		Serial.print(packet[i], HEX);
		Serial.print(" ");
	}
	Serial.print("\r\n");
}

const bool Packet::full() const {
	return byte_index >= packet_length;
}

volatile void Packet::reset() {
	byte_index = 0;
}

/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */

// FIXME: concurrency issues. Research mutexes on Arduino.

PacketBuffer::PacketBuffer(const uint8_t packet_length)
: current_packet(0)
{
	for (int i=0; i<NUM_PACKETS; i++) {
		packets[i].set_packet_length(packet_length);
	}
}

/**
 * @returns true if packet is complete AFTER adding value to it.
 */
volatile const bool PacketBuffer::add(const uint8_t value) {
	packets[current_packet].add(value);

	if (packets[current_packet].full()) {
		if (current_packet >= NUM_PACKETS) {
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

