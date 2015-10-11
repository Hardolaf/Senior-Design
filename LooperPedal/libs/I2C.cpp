/*
 * I2C.cpp
 *
 *  Created on: Oct 11, 2015
 *      Author: Eric Bauer
 */
 
#include "I2C.h"
#include "msp.h"
#include <stdint.h>


/*=====================================
 *       I2C Methods
 *====================================*/
 I2C::~I2C(){
	 // TODO Destructor
 }
 
 I2C::I2C(){
	 // TODO constructor
 }
 
/*=====================================
 *       GPIO I2C Methods
 *====================================*/
 GPIO_I2C::~GPIO_I2C() {
	// TODO Destructor
}

GPIO_I2C::GPIO_I2C(unsigned long i_OUTP, unsigned long i_DIR,
		unsigned long i_IN, uint16_t i_SDA, uint16_t i_SCL) {
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
	 for(mask=0x80;mask;mask >>=1) {
		if((HWREG8(IN & SDA ) != 0) {
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
	unsigned char address = 0x00;
	Start();                          // Start I2C transaction
	address = slave_address | 0x01;
	TX_Byte(slave_address);           // Slave peripheral address
	TX_Byte(register_address);        // peripheral register address
	Stop();
	Start();                          // Start I2C transaction
	TX_Byte(register_address);        
	byte = RX_Byte();                 // Data
	Stop();                           // Stop I2C transaction
	return byte;
}

void GPIO_I2C::WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *tx_array) {
	unsigned int i;
    Start();                           // Start I2C transaction
    TX_Byte(slave_address);            // Slave peripheral address
    TX_Byte(register_address);         // peripheral register address
    for(i=0;i<length;i++)
    {
        TX_Byte(tx_array[i]);          // Data
    }
    Stop();                            // Stop I2C transaction
}

/*=====================================
 *       USCI I2C Methods
 *====================================*/
 
USCI_I2C::~USCI_I2C(){
	// to be implemented
}

USCI_I2C::USCI_I2C(uint16_t i_SDA_PIN, uint16_t i_SCL_PIN){
  SDA_PIN = i_SDA_PIN;
  SCL_PIN = i_SCL_PIN;
  // the following code may need modification for use with the MSP432
  P1SEL |= SDA_PIN + SCL_PIN;	        // Assign I2C pins to USCI_B0
  P1SEL2|= SDA_PIN + SCL_PIN;
  UCB0CTL1 = UCSWRST;					// Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;	// I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;		// Use SMCLK, keep SW reset
  UCB0BR0 = 12;							// Set I2C master speed
  UCB0BR1 = 0;							// Set I2C master speed
  UCB0CTL1 &= ~UCSWRST;					// Clear SW reset, resume operation
}

void USCI_I2C::Write(unsigned char slave_address, unsigned char register_address, unsigned char txdata){
  // the following code may need modification for use with the MSP432
  while (UCB0STAT & UCBBUSY);
  UCB0I2CSA = slave_address;
  UCB0CTL1 |= UCTR + UCTXSTT;            // I2C TX, start condition
  UCB0TXBUF = register_address;			 // transferring register_address
  while (UCB0CTL1 & UCTXSTT);			 // waiting for slaver address was transferred
  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);
  UCB0TXBUF = register_value;			 // transferring one byte value
  while((IFG2 & UCB0TXIFG) != UCB0TXIFG);
  UCB0CTL1 |= UCTXSTP;                   // I2C stop condition
  IFG2 &= ~UCB0TXIFG;                    // Clear USCI_B0 TX int flag
}

void USCI_I2C::WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *tx_array){
  // the following code may need modification for use with the MSP432
  while (UCB0STAT & UCBBUSY);
  Tx_ByteCounter = write_length;
  Tx_Pointer_Temp = Tx_array;
  UCB0I2CSA = slave_address;
  UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
  UCB0TXBUF = register_address; 		  // transferring register_address
  while (UCB0CTL1 & UCTXSTT);			  // waiting for slaver address was transferred
  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);
  while (Tx_ByteCounter)
  	{
	  if (IFG2 & UCB0TXIFG)
	  {
              UCB0TXBUF = *Tx_Pointer_Temp;
              while((IFG2 & UCB0TXIFG) != UCB0TXIFG);
              Tx_Pointer_Temp++;
              Tx_ByteCounter--;
      }
	}
  UCB0CTL1 |= UCTXSTP;                     // I2C stop condition
  IFG2 &= ~UCB0TXIFG;                      // Clear USCI_B0 TX int flag
}
unsigned char USCI_I2C::Read(unsigned char slave_address, unsigned char register_address){
  // the following code may need modification for use with the MSP432
  unsigned char Rx_Buffer;
  while (UCB0STAT & UCBBUSY);
  UCB0I2CSA = slave_address;
  UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);
  UCB0TXBUF = register_address;			  // transferring register_address
  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);
  UCB0CTL1 &= ~UCTR;
  UCB0CTL1 |= UCTXSTT;                    // I2C RX, restart condition
  while (UCB0CTL1 & UCTXSTT);             // Loop until I2C STT is sent
  UCB0CTL1 |= UCTXSTP;
  while ((IFG2 & UCB0RXIFG) != UCB0RXIFG);
  Rx_Buffer = UCB0RXBUF;                  // Move RX data to Rx_Data
  IFG2 &= ~UCB0TXIFG;
  return(Rx_Buffer);
}