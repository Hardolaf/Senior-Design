/*
 * main.cpp
 *
 *  Created on: Oct 7, 2015
 *      Author: Joseph Warner
 */

/********************************************************************************************/
/*    Include                                                                               */
/********************************************************************************************/
/* Internal libs */
#include "Audio_AIC.h"
#include "I2C.h"
#include "GPIO_I2C.h"
//#include "USCI_I2C.h"

/* MSP432 standard libs */
#include "msp.h"
#include <stdint.h>

/********************************************************************************************/
/*    Namespaces                                                                            */
/********************************************************************************************/
using namespace LooperPedal;

/********************************************************************************************/
/*    Definitions                                                                           */
/********************************************************************************************/
// GPIO I2C Definitions
#define GPIO_I2C_OUTP	P2OUT
#define GPIO_I2C_DIR	P2DIR
#define GPIO_I2C_IN		P2IN
#define GPIO_I2C_SCL	BIT6
#define GPIO_I2C_SDA	BIT5
// USCI I2C Definitions
#define USCI_I2C_SDA	BIT7
#define USCI_I2C_SCL	BIT6
// I2C Address Definitions
#define AIC3256			0x30

/********************************************************************************************/
/*                    Main                                                                  */
/********************************************************************************************/
int main() {
	WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
	I2C GPIO = new GPIO_I2C(GPIO_I2C_OUTP, GPIO_I2C_DIR, GPIO_I2C_IN,
			GPIO_I2C_SCL, GPIO_I2C_SDA);
	//I2C USCI = new USCI_I2C(USCI_I2C_SCL, USCI_I2C_SDA);
	Audio_AIC AIC = new Audio_AIC(AIC3256, GPIO);
}
