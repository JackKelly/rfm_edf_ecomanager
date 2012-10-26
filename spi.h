/*
 * Simple functions for communicating with the SPI port.
 *
 *      Author: Jack Kelly
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE
 * LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
 * PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE
 * DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>

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
	byte transfer_byte(const byte& out);

	/**
	 * Send a 16-bit word over the SPI bus, with MSB first.
	 *
	 * @return 16-bit response. First response is MSB.
	 */
	uint16_t transfer_word(const uint16_t& cmd, const bool& ss = true);
};

#endif /* SPI_H_ */
