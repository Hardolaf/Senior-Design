/*
 * GPIOI2C.h
 *
 *  Created on: Oct 7, 2015
 *      Author: Joseph Warner
 */

#ifndef LIBS_GPIOI2C_H_
#define LIBS_GPIOI2C_H_

namespace LooperPedal {

/*
 *
 */
class GPIO_I2C {
public:
	// Destructor
	virtual ~GPIO_I2C();
	// Constructor / Init
	// Make the SCL and SDA High to initialize the I2C
	GPIO_I2C(unsigned long i_OUTP, unsigned long i_DIR,
			unsigned long i_IN, uint16_t i_SDA, uint16_t i_SCL);
	// Generate a Start Flag in I2C
	void Start();
	// Generate a Stop Flag in I2C
	void Stop();
	// Write one byte
	void TX_Byte(unsigned char byte);
	// Read one byte
	unsigned char RX_Byte();
	// I2C Write Function
	void Write(unsigned char TASadr_w, unsigned char reg, unsigned char txdata);
	// I2C Write Function
	unsigned char Read(unsigned char TASadr_w,unsigned char reg);
	//I2C Multi-byte Write
	void WriteMultiBytes(unsigned char TASadr_w,unsigned char reg,
			unsigned int length, unsigned char *txdata_array);
private:
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

#endif /* LIBS_GPIOI2C_H_ */
