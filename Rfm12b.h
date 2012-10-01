#ifndef RFM12B_H_
#define RFM12B_H_

#include "Packet.h"

class Rfm12b {
public:
	static void enable_rx();

	static void enable_tx();

	/**
	 * Initialise RMF12b using commands translated from
	 * Current Cost EnviR RFM01 commands.
	 */
	static void init_cc ();

	/**
	 * Init the RFM12b using commands sniffed from an EcoManager.
	 */
	static void init_edf ();

	/**
	 * If data has been received (and hence rx_packet_buffer is not
	 * empty) then print it to the serial port
	 * and empty rx_packet_buffer.
	 * The serial port must be initialised first.
	 */
	static void print_if_data_available();

	/**
	 * Ping an EDF Wireless Transmitter Plug to ask for
	 * the latest wattage reading.  At the moment the
	 * EDF IAM's ID is hard-coded.
	 */
	static void poll_edf_iam(const uint32_t& uid);

	/**
	 * Mimick a Current Cost CT clamp.
	 * At the moment the packet is hard-coded.
	 */
	static void mimick_cc_ct();

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

};

#endif /* RFM12B_H_ */
