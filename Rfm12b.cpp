#include "Rfm12b.h"
#include <Arduino.h>
#include "spi.h"

Rfm12b::State Rfm12b::state;
Packet Rfm12b::tx_packet;
PacketBuffer Rfm12b::rx_packet_buffer(12);

void Rfm12b::enable_rx()
{
	// 3. Power Management Command 0x8201
	// 1 0 0 0   0 0 1 0  er ebb et es ex eb ew dc
	//                     0   0  0  0  0  0  0  1
	// er : enable whole receiver chain (automatically turns on crystal,
	//      synth, baseband and RF front end)
	// ebb: enable RX baseband circuit (missing on RFM01)
	// et : enable TX (PLL & PA)
	// es : enable synthesiser (must be on to enable baseband circuits)
	// ex : enable crystal oscillator
	// eb : enable low batt detector
	// ew : enable wake-up timer
	// dc : disable clock output of CLK pin
	//                          eeeeeeed
	//                          rbtsxbwc
	spi::transfer_word(0x82D9);
	state = RX;
	reset_fifo();
}

void Rfm12b::enable_tx()
{
	// See power managament command in enable_rx()
	spi::transfer_word(0x8239);
	state = TX;
	Serial.println("TX:");
}

void Rfm12b::tx_next_byte()
{
	const uint8_t out = tx_packet.get_next_byte();
	spi::transfer_word(0xB800 + out);

	Serial.print(out, HEX); // TODO: remove after debugging
	Serial.print(" "); // TODO: remove after debugging

	if (tx_packet.done()) {
		// we've finished transmitting the packet
		Serial.print("\r\n");  // TODO: remove after debugging
		enable_rx();
	}
}

void Rfm12b::reset_fifo()
{

	// To restart synchron pattern recognition, bit ff should be cleared and set
	// i.e. packet will start with preable, then synchron pattern, then packet
	// data, then turn FIFO off and on (I think)

	// 8. FIFO and Reset Mode Command (CA81)
	// 1 1 0 0 1 0 1 0 f3 f2 f1 f0 sp al ff dr
	//                  1  0  0  0  0  0  0  1
	// f: FIFO interrupt level = 8 (RFM01 & default)
	// sp: length of synchron pattern (not on RFM01!!!)
	//     0 = 2 bytes (first = 2D, second is configurable defaulting to D4)
	// al: FIFO fill start condition. Default = sync-word.
	//     0=synchron pattern
	//     1=always fill
	// ff: enable FIFO fill
	// dr: disable hi sensitivity reset mode

	spi::transfer_word(0xCA81); // from EDF config
	spi::transfer_word(0xCA83); // my attempt to enable FIFO fill

}

void Rfm12b::interrupt_handler()
{
	spi::select(true);
	const uint8_t status_MSB = spi::transfer_byte(0x00); // get status word MSB
	spi::transfer_byte(0x00); // get status word LSB

	if (state == RX) {
		bool full = false; // is the buffer full after receiving the byte waiting for us?
		if ((status_MSB & 0x20) != 0) { // FIFO overflow
			full  = rx_packet_buffer.add(spi::transfer_byte(0x00)); // get 1st byte of data
			full |= rx_packet_buffer.add(spi::transfer_byte(0x00));
		} else 	if ((status_MSB & 0x80) != 0) { // FIFO has 8 bits ready
			full = rx_packet_buffer.add(spi::transfer_byte(0x00)); // get 1st byte of data
		}
		spi::select(false);

		if (full) {
			reset_fifo();
		}
	} else { // state == TX
		if ((status_MSB & 0x80) != 0) { // TX register ready
			tx_next_byte();
		}
		spi::select(false);
	}

}

void Rfm12b::init_cc () {
	Serial.println("Starting rf12_initialize_cc() ");

	spi::init();

	spi::transfer_word(0x0000);

	delay(2000); // give RFM time to start up

	Serial.println("RFM12b finished power-up reset.  Starting init...");

	// From RFM01 command #1 0x892D
	// eb=0 (disable low batt detection)
	// et=0 (disable wake-up timer)
	// ex=1 (enable crystal oscillator)
	// baseband bandwidth = 67kHz
	// dc=1 (disable signal output of CLK pin)

	// RFM01 command #2 E196 (5. wake-up timer command)

	// RFM01 command #3 CC0E (6. low duty-cycle command)
	// en = 0: disable low duty cycle mode

	// From RFM01 command #4 C69F (8. AFC Command)
	// a1 a0 rl1 rl0 st fi oe en
	//  1  0   0   1  1  1  1  1
	// a  = AFC auto-mode: keep offset when VDI hi
	// rl = range limit: +15/-16 (433band: 2.5kHz)
	// st=1 st goes hi will store offset into output register
	// fi=1 Enable AFC hi accuracy mode
	// oe=1 Enable AFC output register
	// en=1 Enable AFC function

	// From RFM01 command #5 C46A (9. data filter command)
	// al ml 1 s1 s0 f2 f1 f0
	//  0  1 1  0  1  0  1  0
	// al=0: disable clock recovery auto-lock
	// ml=1: enable clock recovery fast mode
	// s: data filter=digital filter
	// f: DQD threshold = 2

	// From RFM command #6 C88A
	// 3918.5 bps

	// From RFM01 command #7 C080 (4. receiver setting command)
	// d1 d0 g1 g0 r2 r1 r0 en
	//  1  0  0  0  0  0  0  0
	// d: VDI source = clock recovery lock output
	// g: LNA gain = 0 dBm
	// r: DRSSI threshold = -103 dBm
	// en=0: disable receiver

	// GanglionTwitch has command CE88 here, my CC doesn't (11. output and FIFO mode)

	// From RFM01 command #8 CE8B (11. output and FIFO mode)
	// f3 f2 f1 f0 s1 s0 ff fe
	//  1  0  0  0  1  0  1  1
	// f: FIFO interrupt level = 8
	// s: FIFO fill start condition = reserved
	// ff=1: enable FIFO fill
	// fe=1: enable FIFO function

	// From RFM01 command #9 C081 (4. receiver setting command)
	// d1 d0 g1 g0 r2 r1 r0 en
	//  1  0  0  0  0  0  0  1
	// d: VDI source = clock recovery lock output
	// g: LNA gain = 0 dBm
	// r: DRSSI threshold = -103 dBm
	// en=1: enable receiver <--- only diff from command #7

	// From RFM01 command #10 C200 (7. Low Batt Detector & MCU Clock Div)
	// d2 d1 d0 t4 t3 t2 t1 t0
	//  0  0  0  0  0  0  0  0
	// d: frequency of CLK pin = 1MHz
	// t: low batt detection theshold = 2.2+0 V

	// From RFM01 command #11 A618 (3. frequency setting command)
	// Fc = 433.9MHz

	// From RFM01 command #12 CE89 (11. output and FIFO mode) (gangliontwitch has CE88)
	// f3 f2 f1 f0 s1 s0 ff fe
	//  1  0  0  0  1  0  0  1
	// f: FIFO interrupt level = 8
	// s: FIFO fill start condition = reserved
	// ff=0: disable FIFO fill
	// fe=1: enable FIFO function

	// From RFM01 command #14 CE8B (11. output and FIFO mode)
	// f3 f2 f1 f0 s1 s0 ff fe
	//  1  0  0  0  1  0  1  1
	// f: FIFO interrupt level = 8
	// s: FIFO fill start condition = reserved
	// ff=1: enable FIFO fill
	// fe=1: enable FIFO function

	// Reset Mode Command is not set so it defaults to DA00
	// (do not disable highly sensitive reset)

	/***************************
	 * BEGIN RFM12b COMMANDS...
	 ***************************/

	spi::transfer_word(0x0000);

	// 2. configuration setting command
	// 1 0 0 0 0 0 0 0 el ef b1 b0 x3 x2 x1 x0
	// el: enable TX register
	// ef: enable RX FIFO register
	// b: select band. 01 = 433MHz
	// x: load capacitor.
	//    0010 (0x2)=9.5pF (from CC RFM01)
	//    0111 (0x7)=12.0pF (from jeelib)
	//
	//                           ee
	//                   10000000lfbbxxxx
	spi::transfer_word(0b1000000001010111);

	// 3. Power Management Command
	// 1 0 0 0   0 0 1 0  er ebb et es ex eb ew dc
	// er : enable whole receiver chain (automatically turns on crystal,
	//      synth, baseband and RF front end)
	// ebb: enable RX baseband circuit (missing on RFM01)
	// et : enable TX (PLL & PA)
	// es : enable synthesiser (must be on to enable baseband circuits)
	// ex : enable crystal oscillator
	// eb : enable low batt detector
	// ew : enable wake-up timer
	// dc : disable clock output of CLK pin
	//                           eeeeeeed
	//                           rbtsxbwc
	spi::transfer_word(0b1000001011011001);

	// 4. Frequency setting command
	// 1 0 1 0 F
	// F  = 1560 decimal = 0x618
	// Fc = 10 x 1 x (43 + F/4000) MHz = 433.9 MHz
	//
	spi::transfer_word(0xA618); // 433.9MHz

	// 5. Data Rate command
	// 1 1 0 0   0 1 1 0   cs  r...
	// r  = 10 decimal
	// cs = 1
	// BitRate = 10000 / 29 / (R+1) / (1 + 7) = 3.918 kbps (from CCRFM01)
	spi::transfer_word(0xC68A);


	// 6 Receiver control command
	// 1 0 0 1 0 P16 d1 d0 i2 i1 i0 g1 g0 r2 r1 r0
	//
	// p16: function of pin16. 1 = VDI output
	// d: VDI response time. 00=fast, 01=med, 10=slow, 11=always on
	// i: baseband bandwidth. 110=67kHz (CCRFM01=67kHz)
	// g: LNA gain. 00=0dB.
	// r: RSSI detector threshold. 000 = -103 dBm
	//
	//                   10010Pddiiiggrrr
	spi::transfer_word(0b1001010011000000);

	// 7. Digital filter command
	// 1 1 0 0 0 0 1 0 al ml 1 s 1 f2 f1 f0
	//
	// al: clock recovery (CR) auto lock control
	//     1=auto, 0=manual (set by ml).
	//     CCRFM01=0.
	// ml: enable clock recovery fast mode. CCRFM01=1
	// s :  data filter. 0=digital filter. (default & CCRFM01)=0
	// f : DQD threshold. CCRFM01=2; but RFM12b manual recommends >4
	//                           am
	//                   11000010ll1s1fff
	spi::transfer_word(0b1100001001101100);


	// 13 PLL setting
	// spi::spi_transfer_word(0xCC77);

	// 8. FIFO and Reset Mode Command
	// 1 1 0 0 1 0 1 0 f3 f2 f1 f0 sp al ff dr
	//
	// f: FIFO interrupt level = 8 (RFM01 & default)
	// sp: length of synchron pattern (not on RFM01!!!)
	// al: FIFO fill start condition. Default = sync-word.
	//     0=synchron pattern
	//     1=always fill
	// ff: enable FIFO fill
	// dr: disable hi sensitivity reset mode
	//
	//                               safd
	//                   11001010ffffplfr
	spi::transfer_word(0b1100101010000001);

	// 8. FIFO and Reset Mode Command
	// 1 1 0 0 1 0 1 0 f3 f2 f1 f0 sp al ff dr
	//
	// f: FIFO interrupt level = 8 (RFM01 & default)
	// sp: length of synchron pattern (not on RFM01!!!)
	// al: FIFO fill start condition. Default = sync-word.
	//     0=synchron pattern
	//     1=always fill
	// ff: enable FIFO fill
	// dr: disable hi sensitivity reset mode
	//
	//                               safd
	//                   11001010ffffplfr
	spi::transfer_word(0b1100101010000011);

	// 9 Synchron pattern command
	// spi::spi_transfer_word(0xCE55);


	// 11. AFC Command
	// 1 1 0 0 0 1 0 0 a1 a0 rl1 rl0 st fi oe en
	// 1 1 0 0 0 1 0 0  1  0   0   1  1  1  1  1
	// a: AFC auto-mode= keep offset when VDI hi (RFM01 and default)
	// rl: range limit= +15/-16 (433band: 2.5kHz)
	// st=1 st goes hi will store offset into output register
	// fi=1 Enable AFC hi accuracy mode (RFM01)
	// oe=1 Enable AFC output register
	// en=1 Enable AFC function
	spi::transfer_word(0xC49F);

	// 12. TX config
	// spi::spi_transfer_word(0x9850); // !mp,9810=30kHz,MAX OUT

	// 15. wake-up timer command
	spi::transfer_word(0xE000); // NOT USE

	// 16. Low duty cycle
	spi::transfer_word(0xC80E); // NOT USE (last bit is 0 -> disable lower duty cycle mode)

	// 17. low battery detector and micro controller clock div
	spi::transfer_word(0xC000); // 1.0MHz,2.2V

	Serial.println("attaching interrupt");
	attachInterrupt(0, interrupt_handler, LOW);

	Serial.println("Done init.");
	return;
}

void Rfm12b::init_edf () {
	Serial.println("Starting rf12_initialize_edf() ");

	spi::init();

	spi::transfer_word(0x0000);
	// TODO: do we need a software reset here?

	delay(2000); // give RFM time to start up

	Serial.println("RFM12b finished power-up reset.  Starting init...");

	/***************************
	 * BEGIN RFM12b COMMANDS...
	 ***************************/

	// 2. configuration setting command 80D8
	// 1 0 0 0 0 0 0 0 el ef b1 b0 x3 x2 x1 x0
	//                  1  1  0  1  1  0  0  0
	// el: enable TX register (same as CC RFM01)
	// ef: enable RX FIFO register (same as CC RFM01)
	// b:  select band. 01 = 433MHz (same as CC RFM01)
	// x:  load capacitor.
	//      0010 (0x2)=9.5pF  (from EnviR RFM01)
	//      0111 (0x7)=12.0pF (from jeelib)
	//      1000 (0x8)=12.5pF (from EDF EcoManager)
	spi::transfer_word(0x80D8);

	// 3. Power Management Command 0x8201
	// 1 0 0 0   0 0 1 0  er ebb et es ex eb ew dc
	//                     0   0  0  0  0  0  0  1
	// er : enable whole receiver chain (automatically turns on crystal,
	//      synth, baseband and RF front end)
	// ebb: enable RX baseband circuit (missing on RFM01)
	// et : enable TX (PLL & PA)
	// es : enable synthesiser (must be on to enable baseband circuits)
	// ex : enable crystal oscillator
	// eb : enable low batt detector
	// ew : enable wake-up timer
	// dc : disable clock output of CLK pin
	spi::transfer_word(0x8201);

	// 4. Frequency setting command
	// 1 0 1 0 F
	// F  = 1588 decimal = 0x634 (EnviR RFM01 has 1560 decimal)
	// Fc = 10 x 1 x (43 + F/4000) MHz = 433.97 MHz (EnviR RFM01 uses 433.9 MHz)
	spi::transfer_word(0xA634);

	// 5. Data Rate command
	// 1 1 0 0   0 1 1 0   cs  r...
	// r  = 0b1010111 = 87 decimal
	// cs = 0
	// BitRate = 10000 / 29 / (R+1) = 3.918 kbps (same as CC RFM01)
	spi::transfer_word(0xC657);

	// 6 Receiver control command 94C0
	// 1 0 0 1 0 P16 d1 d0 i2 i1 i0 g1 g0 r2 r1 r0
	//             1  0  0  1  1  0  0  0  0  0  0
	// p16: function of pin16. 1 = VDI output (same as CC RFM01)
	// d: VDI response time. 00=fast, 01=med, 10=slow, 11=always on (same as CC RFM01)
	// i: baseband bandwidth. 110=67kHz (same as CC RFM01)
	// g: LNA gain. 00=0dB. (same as CC RFM01)
	// r: RSSI detector threshold. 000 = -103 dBm (same as CC RFM01)
	spi::transfer_word(0x94C0);

	// 7. Digital filter command C22C
	// 1 1 0 0 0 0 1 0 al ml 1 s 1 f2 f1 f0
	//                  0  0 1 0 1  1  0  0
	// al: clock recovery (CR) auto lock control (same as CC RFM01)
	//     1=auto, 0=manual (set by ml).
	//     CCRFM01=0.
	// ml: enable clock recovery fast mode. CCRFM01=1 (diff to CC RFM01)
	// s :  data filter. 0=digital filter. (default & CCRFM01)=0 (same as CC RFM01)
	// f : DQD threshold. CCRFM01=2; but RFM12b manual recommends >4 (diff to CC RFM01)
	spi::transfer_word(0xC22C);

	// 8. FIFO and Reset Mode Command (CA81)
	// 1 1 0 0 1 0 1 0 f3 f2 f1 f0 sp al ff dr
	//                  1  0  0  0  0  0  0  1
	// f: FIFO interrupt level = 8 (RFM01 & default)
	// sp: length of synchron pattern (not on RFM01!!!)
	//     0 = 2 bytes (first = 2D, second is configurable defaulting to D4)
	// al: FIFO fill start condition. Default = sync-word.
	//     0=synchron pattern
	//     1=always fill
	// ff: enable FIFO fill
	// dr: disable hi sensitivity reset mode
	spi::transfer_word(0xCA81); // from EDF config

	// 9. Synchron Pattern Command CED4
	// D4 = default synchron pattern
	spi::transfer_word(0xCED4);

	// 11. AFC Command C4F7
	// 1 1 0 0 0 1 0 0 a1 a0 rl1 rl0 st fi oe en
	// 1 1 0 0 0 1 0 0  1  1   1   1  0  1  1  1
	// a:  AFC auto-mode selector (different to CC RFM01)
	//     10 = keep offset when VDI hi (RFM01 and default)
	//     11 = Keep the f_offset value independently from the state of the VDI signal (EDF EcoManager)
	// rl: range limit (different to CC RFM01)
	//     01 = +15 to -16 (433band: 2.5kHz) (CC RFM01)
	//     11 =  +3 to  -4 (EDF EcoManager)
	// st: (different to CC RFM01)
	//     Strobe edge, when st goes to high, the actual latest
	//     calculated frequency error is stored into the offset
	//     register of the AFC block.
	// fi: Enable AFC hi accuracy mode (same as CC RFM01)
	//     Switches the circuit to high accuracy (fine) mode.
	//     In this case, the processing time is about twice as
	//     long, but the measurement uncertainty is about half.
	// oe: Enables the frequency offset register. (same as CC RFM01)
	//     It allows the addition of the offset register
	//     to the frequency control word of the PLL.
	// en: Enable AFC function. (Same as CC RFM01)
	//     Enables the calculation of the
	//     offset frequency by the AFC circuit.
	spi::transfer_word(0xC4F7);

	// 12. TX Configuration Control Command 0x9820
	// 1 0 0 1 1 0 0 mp m3 m2 m1 m0 0 p2 p1 p0
	// 1 0 0 1 1 0 0  0  0  0  1  0 0  0  0  0
	// mp: FSK modulation parameters (using jeelabs RFM12b calc)
	//     frequency shift = pos
	//     deviation       = 45 kHz
	// p : Output power. 0x000=0dB
	spi::transfer_word(0x9820);

	// 13. PLL Setting Command 0xCC66
	// 1 1 0 0 1 1 0 0 0 ob1 ob0 1 dly ddit 1 bw0
	// 1 1 0 0 1 1 0 0 0   1   1 0   0    1 1   0
	// ob: Microcontroller output clock buffer rise and
	//     fall time control. The ob1-ob0 bits are changing
	//     the output drive current of the CLK pin. Higher
	//     current provides faster rise and fall times but
	//     can cause interference.
	//     11 = 5 or 10MHz (recommended)
	// dly: switches on the delay in the phase detector
	// ddit: disables dithering in the PLL loop
	// bw0: PLL bandwidth set fo optimal TX RM performance
	//      0 = Max bit rate = 86.2kbps
	spi::transfer_word(0xCC66);

	// 15. Wake-up timer command
	// R=0, M=0 (i.e. Twakeup = 0ms, the minimum)
	spi::transfer_word(0xE000);

	// 16. Low Duty-Cycle disabled
	spi::transfer_word(0xC800);

	// 17. Low Batt Detector and Microcontroller Clock Div
	// Low batt threshold = 2.2V (lowest poss)
	// Clock pin freq = 1.0Mhz (lowest poss)
	spi::transfer_word(0xC000);

	Serial.println("attaching interrupt");
	delay(500);
	attachInterrupt(0, interrupt_handler, LOW);
	return;
}

void Rfm12b::print_if_data_available()
{
	if (rx_packet_buffer.data_is_available()) {
		rx_packet_buffer.print_and_reset();
	}
}

void Rfm12b::poll_edf_iam(const uint32_t& uid)
{
	uint8_t tx_data[] = {0x46, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x50, 0x53, 0x00, 0x00, 0x4F};

	// convert 32-bit uid into single bytes
	tx_data[1] = (uid & 0xFF000000) >> 24;
	tx_data[2] = (uid & 0x00FF0000) >> 16;
	tx_data[3] = (uid & 0x0000FF00) >>  8;
	tx_data[4] = (uid & 0x000000FF);

	tx_packet.assemble(tx_data, 11, true);
	enable_tx();
}

void Rfm12b::mimick_cc_ct()
{
	const uint8_t tx_data[] = {0x55, 0x55, 0x65, 0xA6, 0x95, 0x55, 0x55,
			0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
	tx_packet.assemble(tx_data, 16);
	enable_tx();
}
