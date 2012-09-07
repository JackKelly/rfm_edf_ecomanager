// Pins
#define RFM_IRQ      2 // PD2
#define SS_DDR      DDRB
#define SS_PORT     PORTB
#define SS_BIT       2 // for PORTB: 2 = d.10, 1 = d.9, 0 = d.8

#define SPI_SS      10 // PB2, pin 16 (confirmed from Nanode RF schematic)
#define SPI_MOSI    11 // PB3, pin 17
#define SPI_MISO    12 // PB4, pin 18
#define SPI_SCK     13 // PB5, pin 19

namespace spi {
	/* Select or de-select the RFM.
	 *
	 * @param state: if true then select the RFM
	 *               (ready for data transfer)
	 */
	void select(const bool state);

	/**
	 * Initialise the ATmega's SPI hardware for use with the RFM.
	 */
	void init();

	/**
	 * Send a byte over the SPI bus.
	 *
	 * @return 8-bit response
	 */
	const uint8_t transfer_byte(const uint8_t& out);

	/**
	 * Send a 16-bit word over the SPI bus, with MSB first.
	 *
	 * @return 16-bit response. First response is MSB.
	 */
	const uint16_t transfer_word(const uint16_t& cmd, const bool& ss = true);
};
