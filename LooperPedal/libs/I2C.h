/*
 * I2C.h
 *
 *  Created on: Oct 11, 2015
 *      Author: Eric Bauer
 */

#ifndef LIBS_I2C_H_
#define LIBS_I2C_H_

namespace LooperPedal {

class I2C{
public:
	// Deconstructor
	virtual ~I2C();
	// Constructor
	I2C();	
	// I2C Write Function
	virtual void Write(unsigned char slave_address, unsigned char register_address, unsigned char txdata) = 0;
	// I2C Multi-byte Write
	virtual void WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *tx_array) = 0;
	// I2C Read Function
	virtual unsigned char Read(unsigned char slave_address, unsigned char register_address) = 0;
private:
};

} /* namespace LooperPedal */

#endif /* LIBS_I2C_H_ */
