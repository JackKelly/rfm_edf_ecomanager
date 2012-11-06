
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
: length(MAX_PACKET_LENGTH), byte_index(0) {}


Packet::~Packet() {}


void Packet::set_packet_length(const index_t& _packet_length)
{
	length = _packet_length;
}


void Packet::append(const byte bytes[], const index_t& length)
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

	for (int i=0; i<length; i++) {
		Serial.print(packet[i], HEX);
		Serial.print(F(" "));
	}

	Serial.println(F(""));
}


bool Packet::done() const
{
	return byte_index >= length;
}


void Packet::reset() {
	byte_index = 0;
}


byte Packet::modular_sum(
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


#ifdef TESTING
    const volatile index_t& Packet::get_length() const
    {
        return length;
    }

    const volatile byte* Packet::get_packet() const
    {
        return packet;
    }
#endif


/**********************************************
 * TXPacket
 **********************************************/

byte TXPacket::get_next_byte()
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
                length  = CC_TRX_PACKET_LENGTH;
			} else {
                tx_type = TX;
                length  = CC_TX_PACKET_LENGTH;
			}
		}
		packet[byte_index++] = value;
	}
}


void RXPacket::print_id_and_watts() const
{
    print_id_and_type();

	Serial.print(F(", \"t\": "));
	Serial.print(timecode);

	print_sensors();

	if (tx_type == TRX) {
	    Serial.print(F(", \"state\": "));
	    Serial.print(packet[10]==0x53 ? F("1") : F("0"));
	}

	Serial.println(F("}"));
}


void RXPacket::print_id_and_type(const bool on_its_own) const
{
    Serial.print(F("{\"type\": \""));
    Serial.print(tx_type == TX ? F("tx") : F("trx"));
    Serial.print(F("\", \"id\": ")); // {"type": "tx", "id": 123, "t": 1000, "sensors": {0: 100, 1: 500}}
    Serial.print(id);
    if (on_its_own) Serial.print(F("}"));
}


void RXPacket::print_sensors() const
{
    Serial.print(F(", \"sensors\": {"));

    bool first = true;
    for (index_t i=0; i<3; i++) {
        if (watts[i]!=WATTS_INVALID) {
            if (first) first = false;  else Serial.print(F(", "));
            Serial.print(F("\""));
            Serial.print(i+1);
            Serial.print(F("\": "));
            Serial.print(watts[i]);
        }
    }

    Serial.print(F("}"));
}


RXPacket::Health RXPacket::verify_checksum() const
{
	const byte calculated_checksum = modular_sum(packet, length-1);
	return (calculated_checksum == packet[length-1]) ? OK : BAD;
}


void RXPacket::reset()
{
    byte_index = 0;
    health = NOT_CHECKED;
}


bool RXPacket::is_ok()
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


bool RXPacket::is_pairing_request() const
{
    return tx_type == TX ?
            packet[0] & 0b10000000 : // TX
            packet[6]==0x43 && packet[7]==0x4F; // TRX
}


RXPacket::Health RXPacket::de_manchesterise()
{
	const byte ONE  = 0b10000000; // 1 in Manchester-speak is 10
	const byte ZERO = 0b01000000; // 0 in Manchester-speak is 01
	const byte MASK = 0b11000000; // 2-bit window to select current pit pair

	byte bit, // The output bit encoded by the current source bit pair
	     src_byte, // the source byte we're currently processing
	     src_byte_masked, // the source byte masked to expose only the current pit pair
	     output; // the demanchesterised byte
	index_t src_byte_i, src_byte_offset, bit_pair;
	bool success = true; // true unless we find an illegal bit pair (00 and 11 are illegal)

	for (src_byte_i=0; src_byte_i<length; src_byte_i+=2) {

		output = 0;

		// Decode 2 source bytes into 1 output byte
		for (src_byte_offset=0; src_byte_offset<2; src_byte_offset++) {

		    src_byte = packet[src_byte_i+src_byte_offset];

		    // Decode the 4 bit pairs in src_byte
			for (bit_pair=0; bit_pair<8; bit_pair+=2) {
				src_byte_masked = src_byte & (MASK >> bit_pair);
				if (src_byte_masked == ONE >> bit_pair) {
				    bit = 1;
				} else if (src_byte_masked == ZERO >> bit_pair) {
				    bit = 0;
				} else {
				    success = false;
				    bit = 0;
				}
				output <<= 1; // bit-shift output 1 to the left
				output |= bit;
			}
		}
		packet[src_byte_i / 2] = output;
	}

	length /= 2;

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


bool PacketBuffer::append(const byte& value)
{
	packets[current_packet].append(value);

	if (packets[current_packet].done()) {
	    bool successfully_found_empty_slot = false;
	    for (index_t i=0; i<PACKET_BUF_LENGTH; i++) { // find empty slot
	        if (!packets[i].done()) {
	            current_packet = i;
	            successfully_found_empty_slot = true;
	            break;
	        }
	    }
	    if (!successfully_found_empty_slot) {
	        log(ERROR, PSTR("NO MORE BUFFERS!"));
	    }

		return true;
	} else {
		return false;
	}
}
