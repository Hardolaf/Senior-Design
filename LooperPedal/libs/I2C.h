/*
 * I2C.h
 *
 *  Created on: Oct 11, 2015
 *      Author: Eric Bauer
 */

 
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

class GPIO_I2C: public I2C {
public:
	// Deconstructor
	virtual ~GPIO_I2C();
	// Constructor
	GPIO_I2C(unsigned long i_OUTP, unsigned long i_DIR, unsigned long i_IN, uint16_t i_SDA, uint16_t i_SCL);
	// I2C Write Function
	void Write(unsigned char slave_address, unsigned char register_address, unsigned char txdata);
	// I2C Multi-byte Write
	void WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *tx_array);
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

class USCI_I2C: public I2C {
public:
	// Deconstructor
	virtual ~USCI_I2C();
	// Constructor
	USCI_I2C(uint16_t i_SDA_PIN, uint16_t i_SCL_PIN);
	// I2C Write Function
	void Write(unsigned char slave_address, unsigned char register_address, unsigned char txdata);
	// I2C Multi-byte Write
	void WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *tx_array);
	// I2C Read Function
	unsigned char Read(unsigned char slave_address, unsigned char register_address);
private:
	// SDA and SCL pin numbers
	uint16_t SDA_PIN;
	uint16_t SCL_PIN;
};