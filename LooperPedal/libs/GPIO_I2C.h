/*
 * GPIO_I2C.h
 *
 *  Created on: Oct 13, 2015
 *      Author: warne_000
 */

#ifndef LIBS_GPIO_I2C_H_
#define LIBS_GPIO_I2C_H_

#include "I2C.h"
#include "msp.h"
#include <stdint.h>

namespace LooperPedal {

class GPIO_I2C: public I2C {
public:
	// Deconstructor
	virtual ~GPIO_I2C();
	// Constructor
	GPIO_I2C(unsigned long i_OUTP,
			unsigned long i_DIR,
			unsigned long i_IN,
			uint16_t i_SDA,
			uint16_t i_SCL) : I2C()
	{
		// Set internal variables
		OUTP = i_OUTP;
		DIR = i_DIR;
		IN = i_IN;
		SDA = i_SDA;
		SCL = i_SCL;

		// Initialize the port
		// Set output to low
		HWREG8(OUTP) &= ~(SDA | SCL);
		// Set direction of SDA and SCL to input to drive it high through pullup
		HWREG8(DIR) &= ~(SDA | SCL);
	}

	// I2C Write Function
	void Write(unsigned char slave_address,
			unsigned char register_address,
			unsigned char txdata);
	// I2C Multi-byte Write
	void WriteMultiBytes(unsigned char slave_address,
			unsigned char register_address,
			unsigned char write_length,
			unsigned char *tx_array);
	// I2C Read Function
	unsigned char Read(unsigned char slave_address, unsigned char register_address);
private:
	/* =========== Methods =========== */
	// Generate a Start Flag in I2C
	void Start();
	// Generate a Stop Flag in I2C
	void Stop();
	// Write one byte
	void TX_Byte(unsigned char byte);
	// Read one byte
	unsigned char RX_Byte();

	/* =========== Members =========== */
	// Hardware registers
	// HWREG8(x)
	unsigned long OUTP;
	unsigned long DIR;
	unsigned long IN;
	// SDA and SCL pin numbers
	uint16_t SDA;
	uint16_t SCL;
};

} /* namespace LooperPedal */

#endif /* LIBS_GPIO_I2C_H_ */
