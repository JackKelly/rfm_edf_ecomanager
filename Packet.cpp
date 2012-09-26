#include "Packet.h"

Packet::Packet(const uint8_t _packet_length)
: packet_length(_packet_length), byte_index(0), packet_ok(false), uid(UID_NOT_VALID)
{
	if (packet_length > MAX_PACKET_LENGTH) {
		Serial.println("ERROR: packet_length > MAX_PACKET_LENGTH!");
	}
}

void Packet::set_packet_length(const uint8_t _packet_length)
{
	packet_length = _packet_length;
}

void Packet::add(const uint8_t value)
{
	if (!done()) {
		if (byte_index==0) {
			if (value==0x55) { // this packet is from a whole house tx
				whole_house_tx = true;
				packet_length  = WHOLE_HOUSE_TX_PACKET_LENGTH;
			} else {
				whole_house_tx = false;
				packet_length  = EDF_IAM_PACKET_LENGTH;
			}
		}
		packet[byte_index++] = value;
	}
}

void Packet::add(const uint8_t* bytes, const uint8_t length)
{
	for (int i=0; i<length; i++) {
		add(bytes[i]);
	}
}

const uint8_t Packet::get_next_byte()
{
	if (!done()) {
		return packet[byte_index++];
	}
	return 0;
}

void Packet::print() const {

	for (int i=0; i<packet_length; i++) {
		Serial.print(packet[i], HEX);
		Serial.print(" ");
	}

	if (packet_ok) {
		Serial.print("OK ");
	} else {
		Serial.print("FAIL ");
	}

	Serial.print(" wattage=");
	for (uint8_t i=0; i<3; i++) {
		Serial.print(watts[i]);
		Serial.print(" ");
	}
	Serial.print(" uid=");
	Serial.print(uid);
	Serial.print(" whole_house=");
	Serial.print(whole_house_tx);

	Serial.print("\r\n");
}

const bool Packet::done() const {
	return byte_index >= packet_length;
}

void Packet::reset() {
	byte_index = 0;
	whole_house_tx = false;
	watts[0] = watts[1] = watts[2] = WATTS_NOT_VALID;
	uid = UID_NOT_VALID;
	packet_ok = false;
	packet_length = EDF_IAM_PACKET_LENGTH;
}

const uint8_t Packet::modular_sum(
		const volatile uint8_t payload[],
		const uint8_t length
		)
{
    uint8_t acc = 0;
    for (uint8_t i=0; i<length; i++) {
    	acc += payload[i]; // deliberately overflow
    }
    return acc;
}

void Packet::assemble(const uint8_t payload[], const uint8_t payload_length,
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

const bool Packet::verify_checksum() const
{
	const uint8_t calculated_checksum = modular_sum(packet, packet_length-1);
	return (calculated_checksum == packet[packet_length-1]);
}

void Packet::post_process()
{
	if (whole_house_tx) { // this is a whole-house TX packet
		packet_ok = de_manchesterise();
	} else {
		packet_ok = verify_checksum();
	}

	if (packet_ok) {
		decode_wattage();
		decode_uid();
	}
}

void Packet::decode_wattage()
{
	uint8_t msb;

	for (uint8_t sensor=0; sensor<3; sensor++) {
		watts[sensor] = WATTS_NOT_VALID; // "not valid" value
	}

	if (whole_house_tx) {
		for (uint8_t sensor=0; sensor<3; sensor++) {
			if (packet[2+(sensor*2)] & 0x80) { // plugged in
				msb            = packet[2+(sensor*2)];
				msb           &= 0x7F;  // mask off first bit.
				watts[sensor]  = msb << 8;
				watts[sensor] |= packet[3+(sensor*2)];
			}
		}
	} else {
		watts[0]  = packet[9] << 8;
		watts[0] |= packet[8];
	}

}

void Packet::decode_uid()
{
	uid=0;

	if (whole_house_tx) {
		uid |= (packet[0] & 0x0F) << 8; // get nibble from first byte
		uid |= packet[1];
	} else {
		uid |= packet[1] << 24;
		uid |= packet[2] << 16;
		uid |= packet[3] <<  8;
		uid |= packet[4];
	}
}


const bool Packet::de_manchesterise()
{
	const uint8_t ONE  = 0b10000000;
	const uint8_t ZERO = 0b01000000;
	uint8_t mask;
	uint8_t is_one;
	bool success = true;

	for (uint8_t in_byte_i=0; in_byte_i<packet_length; in_byte_i+=2) {
		uint8_t output = 0; // the demanchesterised byte
		for (uint8_t in_byte_offset=0; in_byte_offset<2; in_byte_offset++) {
			for (uint8_t bit_pair=0; bit_pair<4; bit_pair++) {
				mask   = ONE >> bit_pair*2;
				if ((packet[in_byte_i+in_byte_offset] & mask) == mask) {
					is_one = true;
				} else {
					mask   = ZERO >> bit_pair*2;
					if ((packet[in_byte_i+in_byte_offset] & mask) == mask) {
						is_one = false;
					} else {
						// de-manchesterisation failed
						success = false;
					}
				}
				output = output << 1;
				output |= is_one;
			}
		}
		packet[in_byte_i / 2] = output;
	}

	packet_length /= 2;
	return success;
}

const bool Packet::is_ok() const
{
	return packet_ok;
}


// FIXME: concurrency issues? Research mutexes on Arduino.
PacketBuffer::PacketBuffer(const uint8_t packet_length)
: current_packet(0)
{
	for (int i=0; i<NUM_PACKETS; i++) {
		packets[i].set_packet_length(packet_length);
	}
}

const bool PacketBuffer::add(const uint8_t value) {
	packets[current_packet].add(value);

	if (packets[current_packet].done()) {
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
	Serial.println("RX:");
	for (int i=0; i<current_packet; i++) {
		packets[i].post_process();
		if (packets[i].is_ok()) {
			packets[i].print();
		}
		packets[i].reset();
	}
	if (packets[current_packet].done()) {
		Serial.println("current_packet");
		packets[current_packet].post_process();
		if (packets[current_packet].is_ok()) {
			packets[current_packet].print();
		}
		packets[current_packet].reset();
	}
	current_packet = 0;
}

const bool PacketBuffer::data_is_available() const {
	return current_packet > 0;
}

