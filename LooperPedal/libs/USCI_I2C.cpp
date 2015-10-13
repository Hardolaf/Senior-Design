/*
 * USCI_I2C.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: warne_000
 */

#include "USCI_I2C.h"
#include "msp.h"
#include <stdint.h>

namespace LooperPedal {

USCI_I2C::~USCI_I2C(){
	// TODO implemented
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

} /* namespace LooperPedal */
