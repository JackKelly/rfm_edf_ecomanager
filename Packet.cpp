
#ifdef TESTING
#include "tests/FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "Logger.h"
#include "Packet.h"

/**********************************************
 * Packet
 **********************************************/

Packet::Packet()
: packet_length(MAX_PACKET_LENGTH), byte_index(0)
{}


void Packet::set_packet_length(const index_t& _packet_length)
{
	packet_length = _packet_length;
}


void Packet::append(const byte* bytes, const index_t& length)
{
	for (int i=0; i<length; i++) {
		Packet::append(bytes[i]);
	}
}


void Packet::append(const byte& value)
{
	if (!done()) {
		packet[byte_index++] = value;
	}
}


void Packet::print_bytes() const
{

	for (int i=0; i<packet_length; i++) {
		Serial.print(packet[i], HEX);
		Serial.print(" ");
	}

	Serial.println("");
}


const bool Packet::done() const
{
	return byte_index >= packet_length;
}

void Packet::reset() {
	byte_index = 0;
}


const byte Packet::modular_sum(
		const volatile byte payload[],
		const byte& length
		)
{
    byte acc = 0;
    for (index_t i=0; i<length; i++) {
    	acc += payload[i]; // deliberately overflow
    }
    return acc;
}

const volatile index_t& Packet::get_byte_index() const
{
	return byte_index;
}

/**********************************************
 * TXPacket
 **********************************************/

const byte TXPacket::get_next_byte()
{
	if (done()) {
	    return 0;
	} else {
		return packet[byte_index++];
	}
}


void TXPacket::assemble(
        const byte payload[],
        const byte& payload_length,
		const bool add_checksum)
{
	const byte HEADER[] = {
			0x55, // Preamble (to allow RX to lock on).
			0x2D, // Synchron byte 0
			0xD4  // Synchron byte 1
	};
	const byte TAIL[] = {0x40, 0x00};

	const byte HEADER_LENGTH = sizeof(HEADER);
	const byte TAIL_LENGTH   = sizeof(TAIL);

	reset();

	append(HEADER, HEADER_LENGTH);

	append(payload, payload_length);
	if (add_checksum) {
		append(modular_sum(payload, payload_length));
	}
	append(TAIL, TAIL_LENGTH);

	set_packet_length(HEADER_LENGTH + payload_length + add_checksum + TAIL_LENGTH);

	byte_index = 0;
}

/**********************************************
 * RXPacket
 **********************************************/

RXPacket::RXPacket()
:Packet(), tx_type(TX), timecode(0), health(NOT_CHECKED), id(ID_INVALID) {}


void RXPacket::append(const byte& value)
{
	if (!done()) {
		if (byte_index==0) { // first byte
			timecode = millis(); // record timecode that first byte received
			if (value==0x52) { // this packet is from a CC_TRX
                tx_type = TRX;
                packet_length  = CC_TRX_PACKET_LENGTH;
			} else {
                tx_type = TX;
                packet_length  = CC_TX_PACKET_LENGTH;
			}
		}
		packet[byte_index++] = value;
	}
}


void RXPacket::print_id_and_watts() const
{
	Serial.print("{id: ");
	Serial.print(id);
	Serial.print(", t: ");
	Serial.print(timecode);

	for (index_t i=0; i<3; i++) {
		if (watts[i]!=WATTS_INVALID) {
			Serial.print(", s");
			Serial.print(i);
			Serial.print(": ");
			Serial.print(watts[i]);
		}
	}

	if (tx_type == TRX) {
	    Serial.print(", state: ");
	    if (packet[10]==0x53) {
	        Serial.print("on");
	    } else {
	        Serial.print("off");
	    }
	}

	Serial.println("}");
}


const RXPacket::Health RXPacket::verify_checksum() const
{
	const byte calculated_checksum = modular_sum(packet, packet_length-1);
	return (calculated_checksum == packet[packet_length-1]) ? OK : BAD;
}


void RXPacket::reset()
{
    byte_index = 0;
    health = NOT_CHECKED;
}

const bool RXPacket::is_ok()
{
    if (health == NOT_CHECKED) {
        post_process();
    }

    return (health == OK);
}


void RXPacket::post_process()
{
    switch (tx_type) {
    case TX:  health = de_manchesterise(); break;
    case TRX: health = verify_checksum(); break;
	}

	if (health == OK) {
		decode_wattage();
		decode_id();
	}
}


const volatile TxType& RXPacket::get_tx_type() const
{
    return tx_type;
}


void RXPacket::decode_wattage()
{
	byte msb;

	// Reset
	for (index_t sensor=0; sensor<3; sensor++) {
		watts[sensor] = WATTS_INVALID; // "not valid" value
	}

	// Decode wattage (TXs and TRXs use different encodings)
	switch (tx_type) {
	case TX:
		for (index_t sensor=0; sensor<3; sensor++) {
			if (packet[2+(sensor*2)] & 0x80) { // plugged in
				msb            = packet[2+(sensor*2)];
				msb           &= 0x7F;  // mask off first bit.
				watts[sensor]  = msb << 8;
				watts[sensor] |= packet[3+(sensor*2)];
			}
		}
		break;
	case TRX:
		watts[0]  = packet[9] << 8;
		watts[0] |= packet[8];
		break;
	}
}


void RXPacket::decode_id()
{
	id=0;

	switch (tx_type) {
	case TX: // this packet is from a CC transmit-only sensor
		id |= (packet[0] & 0x0F) << 8; // get nibble from first byte
		id |= packet[1];
		break;
	case TRX: // this packet is from a CC transceiver (e.g. an EDF IAM)
		id |= (uint32_t)packet[1] << 24;
		id |= (uint32_t)packet[2] << 16;
		id |= (uint16_t)packet[3] <<  8;
		id |= packet[4];
		break;
	}
}


const bool RXPacket::is_pairing_request() const
{
    return tx_type == TX ?
            packet[0] & 0b10000000 : // TX
            packet[6]==0x43 && packet[7]==0x4F; // TRX
}


const RXPacket::Health RXPacket::de_manchesterise()
{
	const byte ONE  = 0b10000000;
	const byte ZERO = 0b01000000;
	byte mask;
	byte is_one = false;
	bool success = true;

	for (index_t in_byte_i=0; in_byte_i<packet_length; in_byte_i+=2) {
		byte output = 0; // the demanchesterised byte
		for (index_t in_byte_offset=0; in_byte_offset<2; in_byte_offset++) {
			for (index_t bit_pair=0; bit_pair<4; bit_pair++) {
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

	return success ? OK : BAD;
}




const id_t& RXPacket::get_id() const
{
	return id;
}


const watts_t* RXPacket::get_watts() const
{
	return watts;
}


volatile const millis_t& RXPacket::get_timecode() const
{
	return timecode;
}

/******************************************
 * PacketBuffer
 ******************************************/

PacketBuffer::PacketBuffer()
: current_packet(0)
{}

const bool PacketBuffer::append(const byte& value)
{
	packets[current_packet].append(value);

	if (packets[current_packet].done()) {
	    bool successfully_found_empty_slot = false;
	    for (index_t i=0; i<NUM_PACKETS; i++) { // find empty slot
	        if (!packets[i].done()) {
	            current_packet = i;
	            successfully_found_empty_slot = true;
	            break;
	        }
	    }
	    if (!successfully_found_empty_slot) {
	        log(ERROR, "NO MORE BUFFERS!");
	    }

		return true;
	} else {
		return false;
	}
}


