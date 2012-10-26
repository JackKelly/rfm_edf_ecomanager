#ifdef ARDUINO
#include <inttypes.h>
#else
#include <Arduino.h>
#endif

#include "spi.h"

void spi::select(const bool state)
{
	if (state == true) {
		bitClear(SS_PORT, SS_BIT);
	} else {
		bitSet(SS_PORT, SS_BIT);
	}
}


void spi::init()
{
  
    select(false);
    bitSet(SS_DDR, SS_BIT);
    digitalWrite(SS, 1);

    /* Set direction register for SCK and MOSI pin.
     * MISO pin automatically overrides to INPUT.
     * When the SS pin is set as OUTPUT, it can be used as
     * a general purpose output port (it doesn't influence
     * SPI operations). */

    pinMode(SPI_SCK, OUTPUT);
    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_SS, OUTPUT);
    pinMode(SPI_MISO, INPUT);
  

    /* SPE enables SPI
     * MSTR instructs AVR to operate in SPI master mode */
    SPCR = _BV(SPE) | _BV(MSTR);

    pinMode(RFM_IRQ, INPUT);
    digitalWrite(RFM_IRQ, 1); // pull-up

	/* RFM12b can cope with a 20MHz SPI clock. Page 13 of the RFM12b manual
	 * gives clock high time and clock low time as 25ns each. Hence total
	 * cycle time is 50ns. Hence max frequency is 1/( 50 x 10^-9 ).
	 * DIV2 is the fastest we can go.
     * https://sites.google.com/site/qeewiki/books/avr-guide/spi
     * But jeelib uses clk/8 for sending so let's use that */
    SPSR |= _BV(SPI2X);
    bitClear(SPCR, SPR1);
	bitSet(SPCR, SPR0);

	/* CPOL=0 (base value of clock is zero)
	 * CPHA=0 (data bits read upon rising edge of clock) */
	bitClear(SPCR, CPOL);
	bitClear(SPCR, CPHA);
}


uint8_t spi::transfer_byte(const uint8_t& out)
{
	SPDR = out;
    // this loop spins 4 usec with a 2 MHz SPI clock
    while (!(SPSR & _BV(SPIF)))
        ; // SPIF goes high when data transfer complete

    return SPDR;
}


uint16_t spi::transfer_word(const uint16_t& cmd, const bool& ss)
{
	if (ss) select(true);
	uint16_t reply = transfer_byte(cmd >> 8) << 8; 	// transfer MSB first
	reply |= transfer_byte(cmd & 0x00FF); // transfer LSB
	if (ss) select(false);
	return reply;
}
