#include "Packet.h"

class Rfm12b {
public:
	static void enable_rx();

	static void enable_tx();

	static void tx_next_byte();

	static void reset_fifo();

	static void interrupt_handler();

	/**
	 * Initialise RMF12b using commands translated from Current Cost EnviR
	 * RFM01 commands.
	 */
	static void init_cc ();

	static void init_edf ();

	static void print_if_data_available();

	static void ping_edf_iam(const uint8_t chksum);

	static void mimick_cc_ct();

private:
	static enum State {RX, TX} state;
	static Packet tx_packet;
	static PacketBuffer rx_packet_buffer;
};
