#include "Packet.h"

class Rfm12b {
public:
	static void enable_rx();

	static void enable_tx();

	static void tx_next_byte();

	/*
	 * TODO: SPI sniff EnviR to see if it picks up my TX
	 */
	static void tx_payload(const uint8_t* payload, const uint8_t payload_length);

	static void reset_fifo();

	static void interrupt_handler();

	/**
	 * Initialise RMF12b using commands translated from Current Cost EnviR
	 * RFM01 commands.
	 */
	static void init_cc ();

	static void init_edf ();

	static void print_if_data_available();

	static void ping_iam();

	static void mimick_cc_ct();

private:
	static enum State {RX, TX} state;
	const static uint8_t packet_length = 22;
	static uint8_t packet[packet_length];
	static uint8_t packet_index;
	static PacketBuffer PACKET_BUFFER;
};
