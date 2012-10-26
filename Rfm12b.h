/*
 *      Author: Jack Kelly
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE
 * LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
 * PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE
 * DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 */

#ifndef RFM12B_H_
#define RFM12B_H_

#include "Packet.h"

class Rfm12b {
public:
	static void enable_rx();

	static void enable_tx();

	/**
	 * Init the RFM12b using commands sniffed from an EcoManager.
	 */
	static void init ();

	/**
	 * Poll a CurrentCost transceiver (TRX), e.g. an EDF Wireless Transmitter Plug,
	 * to ask for the latest wattage reading.
	 */
	static void poll_cc_trx(const id_t& id);

	/**
	 * Send acknowledgement to complete pairing.
	 */
	static void ack_cc_trx(const id_t& id);

	static PacketBuffer rx_packet_buffer;

	static void set_freq_for_cctx();
	static void set_freq_for_cctrx();
	static void enable_afc();
	static void disable_afc();

private:
	static enum State {RX, TX} state; // state RFM12b is in
	static TXPacket tx_packet; // the packet about to be sent

	/**
	 * Get the next byte in Rfm12b::tx_packet
	 * and send it to the RFM12b for transmission.
	 */
	static void tx_next_byte();

	/**
	 * Reset the RFM12b's FIFO buffer. This is necessary
	 * at the end of each packet to tells the RFM12b to
	 * only fill the buffer again if a sync word is received.
	 */
	static void reset_fifo();

	/**
	 * Called every time the RFM12b fires an interrupt request.
	 * This must handle interrupts associated with both RX and TX.
	 */
	static void interrupt_handler();

	static void send_command_to_trx(const byte& cmd1, const byte& cmd2, const id_t& id);

};

#endif /* RFM12B_H_ */
