/*
 * GPIO_I2C.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: warne_000
 */

#include "GPIO_I2C.h"
#include "msp.h"
#include <stdint.h>

namespace LooperPedal {

GPIO_I2C::~GPIO_I2C() {
	// TODO Destructor
}

void GPIO_I2C::Start() {
	// Set direction of SDA and SCL to input to drive it high through pullup
	HWREG8(DIR) &= ~(SDA | SCL);
	// START condition. Drive SDA low while SCL high
	HWREG8(DIR) |= SDA;
	// Then drive SCL low
	HWREG8(DIR) |= SCL;
}

void GPIO_I2C::Stop() {
	HWREG8(DIR) &= ~SCL;
	HWREG8(DIR) &= ~SDA;
}

void GPIO_I2C::TX_Byte(unsigned char byte) {
	unsigned char n, mask = 0x80;

	// Process 8 bits
	for(n=0;n<8;n++) {
		if((byte & mask) != 0) {
			// bit is high
			HWREG8(DIR) &= ~SDA;		// Set SDA

			//Make clock cycle
			HWREG8(DIR) &= ~SCL;	    	// Set SCL
			HWREG8(DIR) |= SCL;	    		// Reset SCL
		} else {
			// bit is low
			HWREG8(DIR) |= SDA;	    		// Reset SDA

			//Make clock cycle
			HWREG8(DIR) &= ~SCL;	    	// Set SCL
			HWREG8(DIR) |= SCL;	    		// Reset SCL
		}
		mask >>= 1;			// shift mask one bit right
	}

	// Acknowledge
	HWREG8(DIR) &= ~SDA;			// Set SDA to input (high)
	n = 0;
	// Wait until I2C device pulls SDA low
	while((HWREG8(IN) & SDA)!= 0) {
		n++;
		if(n == 100) {
			break;
		}
	}

	// Last clock cycle
	HWREG8(OUTP) |= SCL;				// Set SCL
	HWREG8(OUTP) &= ~SCL;		        // Reset SCL
	HWREG8(DIR) |= SDA; 		        // Set SDA to output (low)
	return;
}

unsigned char GPIO_I2C::RX_Byte() {
	unsigned char byte = 0x00;
	unsigned char mask;

	HWREG8(DIR) &= ~SDA;

	// Process 8 bits
	 for(mask=0x80; mask;mask >>=1) {
		if((HWREG8(IN & SDA ) != 0)) {
			// bit is high
			byte |= mask;
			//Make clock cycle
			HWREG8(DIR) &= ~SCL;	    // Set SCL
			HWREG8(DIR) |= SCL;	    	// Reset SCL
		} else {
			// bit is low
			//Make clock cycle
			HWREG8(DIR) &= ~SCL;	    // Set SCL
			HWREG8(DIR) |= SCL;			// Reset SCL
		}
	   // byte <<= 1;			// shift byte one bit right
	}

	// Acknowledge
	HWREG8(DIR)  &= ~SDA;			// Set SDA to input (high)
	HWREG8(OUTP) |= SCL;			// Set SCL
	HWREG8(DIR)  |= SDA; 		    // Set SDA to output (low)
	HWREG8(OUTP) &= ~SCL;		    // Reset SCL
	HWREG8(DIR)  &= ~SDA;			// Set SDA to input (high)
	return byte;
}

void GPIO_I2C::Write(unsigned char slave_address, unsigned char register_address, unsigned char txdata) {
   Start();                          // Start I2C transaction
   TX_Byte(slave_address);           // Slave peripheral address
   TX_Byte(register_address);        // peripheral register address
   TX_Byte(txdata);                  // Data
   Stop();                           // Stop I2C transaction
}

unsigned char GPIO_I2C::Read(unsigned char slave_address, unsigned char register_address) {
	unsigned char byte = 0x00;
	Start();                          // Start I2C transaction
	TX_Byte(slave_address);           // Slave peripheral address
	TX_Byte(register_address);        // peripheral register address
	Stop();
	Start();                          // Start I2C transaction
	TX_Byte(register_address);
	byte = RX_Byte();                 // Data
	Stop();                           // Stop I2C transaction
	return byte;
}

void GPIO_I2C::WriteMultiBytes(unsigned char slave_address,
		unsigned char register_address,
		unsigned char write_length,
		unsigned char *tx_array) {
   Start();                           // Start I2C transaction
   TX_Byte(slave_address);            // Slave peripheral address
   TX_Byte(register_address);         // peripheral register address
   for(unsigned int i=0; i<write_length; i++) {
       TX_Byte(tx_array[i]);          // Data
   }
   Stop();                            // Stop I2C transaction
}


} /* namespace LooperPedal */
