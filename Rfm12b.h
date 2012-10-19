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
	 * Send acknowledgment to complete pairing.
	 */
	static void ack_cc_trx(const id_t& id);

	static PacketBuffer rx_packet_buffer; // TODO: volatile?

private:
	static enum State {RX, TX} state; // state RFM12b is in
	static TXPacket tx_packet; // the packet about to be sent TODO: volatile?

	/**
	 * Get the next byte in Rfm12b::tx_packet
	 * and send it to the RFM12b for transmission.
	 */
	static void tx_next_byte(); // TODO: volatile?

	/**
	 * Reset the RFM12b's FIFO buffer. This is necessary
	 * at the end of each packet to tells the RFM12b to
	 * only fill the buffer again if a sync word is received.
	 */
	static void reset_fifo(); // TODO: volatile?

	/**
	 * Called every time the RFM12b fires an interrupt request.
	 * This must handle interrupts associated with both RX and TX.
	 */
	static void interrupt_handler();

	static void send_command_to_trx(const byte& cmd1, const byte& cmd2, const id_t& id);

};

#endif /* RFM12B_H_ */
