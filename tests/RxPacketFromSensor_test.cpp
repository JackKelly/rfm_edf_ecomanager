/*
 * Packet_test.cpp
 *
 *  Created on: 21 Oct 2012
 *      Author: jack
 */

#include <iostream>
#include <tests/FakeArduino.h>
#include "../RxPacketFromSensor.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PacketTest
#include <boost/test/unit_test.hpp>

void append_array(RxPacketFromSensor& rx_packet,
        const byte data[], const index_t length)
{
    for (index_t i=0; i<length; i++){
        rx_packet.append(data[i]);
    }
}

BOOST_AUTO_TEST_CASE(txPacket1)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x55, 0xA6, 0x6A, 0xAA, 0x95, 0x55, 0x9A, 0x65,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(rx_packet.is_ok());

    BOOST_CHECK_EQUAL(rx_packet.get_tx_type(), CCTX);

    BOOST_CHECK(!rx_packet.is_pairing_request());
    BOOST_CHECK_EQUAL(rx_packet.get_id(), 3455);

    const watts_t* watts = rx_packet.get_watts();
    BOOST_CHECK_EQUAL(watts[0], 180);
    BOOST_CHECK_EQUAL(watts[1], WATTS_INVALID);
    BOOST_CHECK_EQUAL(watts[2], WATTS_INVALID);
}

BOOST_AUTO_TEST_CASE(txPacket2)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x55, 0xA6, 0x6A, 0xAA, 0x95, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(rx_packet.is_ok());

    BOOST_CHECK_EQUAL(rx_packet.get_tx_type(), CCTX);

    BOOST_CHECK(!rx_packet.is_pairing_request());
    BOOST_CHECK_EQUAL(rx_packet.get_id(), 3455);

    const watts_t* watts = rx_packet.get_watts();
    BOOST_CHECK_EQUAL(watts[0], 0);
    BOOST_CHECK_EQUAL(watts[1], WATTS_INVALID);
    BOOST_CHECK_EQUAL(watts[2], WATTS_INVALID);
}

BOOST_AUTO_TEST_CASE(txPacket3)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x55, 0xAA, 0x65, 0x96, 0x95, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(rx_packet.is_ok());

    BOOST_CHECK_EQUAL(rx_packet.get_tx_type(), CCTX);

    BOOST_CHECK(!rx_packet.is_pairing_request());
    BOOST_CHECK_EQUAL(rx_packet.get_id(), 3913);

    const watts_t* watts = rx_packet.get_watts();
    BOOST_CHECK_EQUAL(watts[0], 0);
    BOOST_CHECK_EQUAL(watts[1], WATTS_INVALID);
    BOOST_CHECK_EQUAL(watts[2], WATTS_INVALID);
}

BOOST_AUTO_TEST_CASE(txPacket4)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x95, 0x96, 0x6A, 0x96, 0x95, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(rx_packet.is_ok());

    BOOST_CHECK_EQUAL(rx_packet.get_tx_type(), CCTX);

    BOOST_CHECK(rx_packet.is_pairing_request());
    BOOST_CHECK_EQUAL(rx_packet.get_id(), 2425);

    const watts_t* watts = rx_packet.get_watts();
    BOOST_CHECK_EQUAL(watts[0], 0);
    BOOST_CHECK_EQUAL(watts[1], WATTS_INVALID);
    BOOST_CHECK_EQUAL(watts[2], WATTS_INVALID);
}

BOOST_AUTO_TEST_CASE(txPacket5)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x55, 0x55, 0x65, 0xA6, 0x95, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(rx_packet.is_ok());

    BOOST_CHECK_EQUAL(rx_packet.get_tx_type(), CCTX);

    BOOST_CHECK(!rx_packet.is_pairing_request());
    BOOST_CHECK_EQUAL(rx_packet.get_id(), 77);

    const watts_t* watts = rx_packet.get_watts();
    BOOST_CHECK_EQUAL(watts[0], 0);
    BOOST_CHECK_EQUAL(watts[1], WATTS_INVALID);
    BOOST_CHECK_EQUAL(watts[2], WATTS_INVALID);
}

BOOST_AUTO_TEST_CASE(demanchesterise)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x55, 0x55, 0x65, 0xA6, 0x95, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(rx_packet.is_ok());

    BOOST_CHECK_EQUAL(rx_packet.get_length(), 8);

    const volatile byte* packet = rx_packet.get_packet();

    const byte demanch_target[] = {0x00, 0x4d, 0x80, 0x00,
                                   0x00, 0x00, 0x00, 0x00};

    BOOST_CHECK_EQUAL_COLLECTIONS(packet, packet+8, demanch_target, demanch_target+8);
}


BOOST_AUTO_TEST_CASE(demanchesteriseFail)
{
    RxPacketFromSensor rx_packet;

    const index_t LENGTH = 16;
    const byte data[] = {
            0x57, 0x55, 0x65, 0xA6, 0x95, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55  };

    append_array(rx_packet, data, LENGTH);

    BOOST_CHECK(!rx_packet.is_ok());
}
