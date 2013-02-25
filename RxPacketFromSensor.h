/*
 * RxPacketFromSensor.h
 *
 *  Created on: 12 Dec 2012
 *      Author: jack
 */

#ifndef RXPACKETFROMSENSOR_H_
#define RXPACKETFROMSENSOR_H_

#include <Packet.h>
#include "consts.h"

class RxPacketFromSensor : public RxPacket<> {
public:
    RxPacketFromSensor();
    void print_id_and_watts(const bool reply_to_poll = false) const;
    void print_id_and_type(const bool on_its_own = false) const;
    void print_sensors() const;
    bool is_pairing_request() const;
    const volatile TxType& get_tx_type() const;
    const id_t& get_id() const;
    const watts_t* get_watts() const;

private:
    /********************
     * Consts           *
     * ******************/
    const static byte CC_TRX_PACKET_LENGTH = 12;
    const static byte CC_TX_PACKET_LENGTH  = 16;

    /****************************************************
     * Member variables used within ISR and outside ISR *
     ****************************************************/
    volatile TxType tx_type; // is this packet from a transmit-only sensor (as opposed to a transceiver)?

    /******************************************
     * Member variables never used within ISR *
     ******************************************/
    watts_t watts[3]; // the decoded reading from sensors
    id_t id; // the sensor radio ID

    /********************************************
     * Private methods                          *
     ********************************************/

    void handle_first_byte(const byte& first_byte);

    /**
     * Run this after packet has been received fully to
     * demanchesterise (if from TX), set health, watts and id.
     */
    void post_process();

    /**
     * Decodes watts and sets Packet::watts
     */
    void decode_wattage();

    void decode_id();

    /**
     * De-Manchesterise this packet.
     *
     * Data from CC TXs appears to be "Manchesterised" such that x bytes of
     * Manchesterised data is decoded to produce x/2 bytes of data.
     * A bit pair of 01 in the source encodes a 0 and a 10 encodes a 1.
     * The fact that CC TX data is Manchesterised appears to have been
     * first figured out by gangliontwitch.
     *
     * @return OK if de-manchesterisation went OK
     * @return BAD if any illegal bit pairs (11 or 00) were found
     */
    Health de_manchesterise();

};

#endif /* RXPACKETFROMSENSOR_H_ */
