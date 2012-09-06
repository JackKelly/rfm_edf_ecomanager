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
	if (verify()) {
		Serial.print("OK ");
	} else {
		Serial.print("FAIL ");
	}

	Serial.print("wattage=");
	Serial.print(get_wattage());

	Serial.print("\r\n");
}

const bool Packet::full() const {
	return byte_index >= packet_length;
}

volatile void Packet::reset() {
	byte_index = 0;
}

const uint8_t Packet::modular_sum(volatile const uint8_t payload[], const uint8_t length)
{
    uint8_t acc = 0;
    for (uint8_t i=0; i<length; i++) {
    	acc += payload[i]; // deliberately overflow
    }
    return acc;
}

volatile void Packet::assemble(const uint8_t payload[], const uint8_t payload_length,
		const bool add_checksum)
{
	const uint8_t HEADER[] = {
			0x55, // Preamble (to allow RX to lock on).
			0x2D, // Synchron byte 0
			0xD4  // Synchron byte 1
	};
	const uint8_t TAIL[] = {0x40, 0x00};

	const uint8_t HEADER_LENGTH = sizeof(HEADER);
	const uint8_t TAIL_LENGTH   = sizeof(TAIL);

	reset();
	set_packet_length(HEADER_LENGTH + payload_length + add_checksum + TAIL_LENGTH);

	add(HEADER, HEADER_LENGTH);
	add(payload, payload_length);
	if (add_checksum) {
		add(modular_sum(payload, payload_length));
	}
	add(TAIL, TAIL_LENGTH);

	reset();
}

volatile const bool Packet::verify() const
{
	volatile const uint8_t calculated_checksum = modular_sum(packet, packet_length-1);
	return calculated_checksum == packet[packet_length-1];
}

volatile const uint16_t Packet::get_wattage() const
{
	uint16_t wattage = packet[9] << 8;
	wattage += packet[8];
	return wattage;
}

/**
 * Class for storing multiple packets.  We need this because
 * multiple packets might arrive before we have a chance to
 * read these packets over the FTDI serial port.
 */

// FIXME: concurrency issues? Research mutexes on Arduino.
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

