/*
 * RxPacketFromSensor.cpp
 *
 *  Created on: 12 Dec 2012
 *      Author: jack
 */

#include "RxPacketFromSensor.h"
#include <utils.h>

#ifdef TESTING
#include <tests/FakeArduino.h>
#else
#include <Arduino.h>
#endif // TESTING

RxPacketFromSensor::RxPacketFromSensor()
:tx_type(CCTX), id(ID_INVALID) {}


void RxPacketFromSensor::post_process()
{
    switch (tx_type) {
    case CCTX: health = de_manchesterise(); break;
    case CCTRX: health = verify_checksum(); break;
    }

    if (health == OK) {
        decode_wattage();
        decode_id();
    }
}


void RxPacketFromSensor::handle_first_byte(const byte& first_byte)
{
    if (first_byte==0x52) { // this packet is from a CC_TRX
        tx_type = CCTRX;
        length = CC_TRX_PACKET_LENGTH;
    } else {
        tx_type = CCTX;
        length = CC_TX_PACKET_LENGTH;
    }
}

void RxPacketFromSensor::print_id_and_watts() const
{
    print_id_and_type();

    Serial.print(F(", \"t\": "));
    Serial.print(timecode);

    print_sensors();

    if (tx_type == CCTRX) {
        Serial.print(F(", \"state\": "));
        Serial.print(packet[10]==0x53 ? F("1") : F("0"));
    }

    Serial.println(F("}"));
}


void RxPacketFromSensor::print_id_and_type(const bool on_its_own) const
{
    Serial.print(F("{\"type\": \""));
    Serial.print(tx_type == CCTX ? F("tx") : F("trx"));
    Serial.print(F("\", \"id\": ")); // {"type": "tx", "id": 123, "t": 1000, "sensors": {0: 100, 1: 500}}
    Serial.print(id);
    if (on_its_own) Serial.print(F("}"));
}


void RxPacketFromSensor::print_sensors() const
{
    Serial.print(F(", \"sensors\": {"));

    bool first = true;
    for (index_t i=0; i<3; i++) {
        if (watts[i]!=WATTS_INVALID) {
            if (first) first = false; else Serial.print(F(", "));
            Serial.print(F("\""));
            Serial.print(i+1);
            Serial.print(F("\": "));
            Serial.print(watts[i]);
        }
    }

    Serial.print(F("}"));
}


const volatile TxType& RxPacketFromSensor::get_tx_type() const
{
    return tx_type;
}


void RxPacketFromSensor::decode_wattage()
{
    byte msb;

    // Reset
    for (index_t sensor=0; sensor<3; sensor++) {
        watts[sensor] = WATTS_INVALID; // "not valid" value
    }

    // Decode wattage (TXs and TRXs use different encodings)
    switch (tx_type) {
    case CCTX:
        for (index_t sensor=0; sensor<3; sensor++) {
            if (packet[2+(sensor*2)] & 0x80) { // plugged in
                msb = packet[2+(sensor*2)];
                msb &= 0x7F; // mask off first bit.
                watts[sensor] = msb << 8;
                watts[sensor] |= packet[3+(sensor*2)];
            }
        }
        break;
    case CCTRX:
        watts[0] = packet[9] << 8;
        watts[0] |= packet[8];
        break;
    }
}


void RxPacketFromSensor::decode_id()
{
    switch (tx_type) {
    case CCTX: // this packet is from a CC transmit-only sensor
        id = 0;
        id |= (packet[0] & 0x0F) << 8; // get nibble from first byte
        id |= packet[1];
        break;
    case CCTRX: // this packet is from a CC transceiver (e.g. an EDF IAM)
        id = utils::bytes_to_uint32(packet+1);
        break;
    }
}


bool RxPacketFromSensor::is_pairing_request() const
{
    return tx_type == CCTX ?
            packet[0] & 0b10000000 : // TX
            packet[6]==0x43 && packet[7]==0x4F; // TRX
}


RxPacketFromSensor::Health RxPacketFromSensor::de_manchesterise()
{
    const byte ONE = 0b10000000; // 1 in Manchester-speak is 10
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


const id_t& RxPacketFromSensor::get_id() const
{
    return id;
}


const watts_t* RxPacketFromSensor::get_watts() const
{
    return watts;
}

