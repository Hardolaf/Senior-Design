//*************************************************************************************
//  							MSP430G2553 Demo
// main.c - Launchpad Tools for Audio Demo's and Development. By Derek Xie June 2013.
// Hardware Requirements  - TI MSP430 Launchpad with MSP430G2553 onboard.
//
//  Description: This software offers 2 functions:
// 1 - Boot from I2C header file, for self booting demo's etc from header files created by TI tools.
// 2 - Real time I2C debug port through the USB interface on launchpad. Allowing real time read-write of I2C.
// Please see the documentation in the product folder at TI.com for details.* (Not complete yet)
//
// There are two different kinds of I2C write available in this implimentation
// One for miniDSP devices (such as AIC3254,PCM514x etc) and non-miniDSP devices (e.g. TAS5717, PCM9211 etc)
// miniDSP devices require some additional logic to decode the I2C addresses into additoinal delay instructions etc.
// This is because of the architecture of the miniDSP and timing requirements etc.
// All other regular devices can use the other the non-AIC i2c writes.
//
// I2C can be driven either by GPIO Pins or by using the USCI on the device.
//
//                MSP430G2553
//             -----------------
//            |                 |
//            |                 |    SCL        -----------------
//            |     P2.1/GPIO	|------------> |                 |
//            |                 | 	 SDA 	   | The Demo Board  |
//            |     P2.0/GPIO	|------------> | We can use GPIO |
//            |                 |			   | I2C or USCI I2C |
//            |                 |    SDA       | to configure    |
//            |     P1.7/UCB0SDA|------------> |                 |
//            |                 | 	 SCL       |                 |
//            |     P1.6/UCB0SCL|------------> |                 |
//            |                 |				-----------------
//            |                 |			    -----------------
//            |     P1.2/UCA0TXD|------------> |                 |
//            |                 | 9600 - 8N1   |	Computer     |
//            |     P1.1/UCA0RXD|<------------ |				 |
//            |                 | 			    -----------------
//            |     P1.0/GPIO   |------------>  LED
//*******************************************************************************************

/********************************************************************************************/
/*    Include                                                                               */
/********************************************************************************************/
#include	"msp430g2553.h"
#include	"I2CBootFile/AIC_File_Conf1.h" 	// This is an example header file from AIC3254 Purepath Studio
#include	"I2CBootFile/type.h"
#include	"I2CBootFile/Tas5717.h"			// This is a example .h I2C array for a TAS5717
#include	"stdbool.h"
#include    "I2CBootFile/AIC_File_Conf2.h"  // This is an example header file from AIC3254 Purepath Studio and used for Button pressed.
											//And the name should not be same as the AIC_File_Conf1.h's, I add 1 at the end of each name.

/********************************************************************************************/
/*    Parameter Definitions                                                                 */
/********************************************************************************************/
unsigned char byte = 0x00;
unsigned int i=0;
unsigned int j=0;
unsigned int h=0;

/********************************************************************************************/
/*    I2C Definitions                                                               	    */
/********************************************************************************************/
//I2C Port Definitions
#define   DIR         P2DIR
#define   OUTP 	      P2OUT
#define   IN          P2IN		//For I2C
//GPIO I2C PIN Definition
#define   SCL  	      BIT1		//Bit 1 GPIO Port 2(SCL)
#define   SDA  	      BIT0		//Bit 0 GPIO Port 2(SDA)
//USCI I2C PIN Definition
#define   SDA_PIN     BIT7		//Bit 7 USCI Port 1(SDA)
#define   SCL_PIN     BIT6		//Bit 6 USCI Port 1(SCL)
//I2C Address Definition
//If use GPIO, you can use D/S's address(0x30). If you USCI, you should modify the address(right-shift). 0x30-->0x18
#define   AIC3254     0x30      //Address GPIO Address
#define   Tas5717     0x54      //Address
//Change this parameter will change the I2C mode. If the GPIO = 1, it will use GPIO I2C, if the GPIO != 1, the code will use USCI I2C.
#define   GPIO         1		//1.GPIO; 2.USCI
//I2C(USCI) Parameter Definition
unsigned char I2C_METHOD;       //I2C Option Parameter
unsigned char Tx_ByteCounter; 	//TX bite counter to save the I2C length
unsigned char Rx_ByteCounter;	//RX bite counter to save the I2C length
unsigned char *Tx_Pointer_Temp; //TX temp array to save the TX data
unsigned char *Rx_Pointer_Temp;	//RX temp array to save the RX data
char *byte_RX;					//I2C read in UART

#define Button1_KEY_FG    0x08  //Button Press Port1 Bit3
unsigned char Button1_State = 0x01;

/********************************************************************************************/
/*    UART Definitions                                                                  */
/********************************************************************************************/
//UART Message
char Tx_message0[]=" 7 bytes received  ";
char Tx_message1[]=" I2C Write command  ";
char Tx_message2[]=" I2C read command ";
char Tx_message3[]=" changing counter value ";
char Tx_message4[]=" Last Character wasn't Enter. Command ignored. ";

unsigned char BitCnt;				// Bit count, used when transmitting byte
char TXByte;						// Value sent over UART when Transmit() is called
char RXByte;						// Value received once hasRecieved is set
unsigned int CurrentNumber; 		// Variable to count button presses.
unsigned char currcommand[8]; 		// current command. 7 bytes plus newline.
char Buffer[2];						// Temp array to storage the value.
unsigned int currcommandbitpoint = 0;	//counter variable for filling the currcommand buffer.

unsigned char ADDR_Device; 			//Device Address in UART
unsigned char ADDR_Register;		//Register Address in UART
unsigned char ADDR_Value;			//Register Value in UART


/********************************************************************************************/
/*    Function Definitions  												                */
/********************************************************************************************/
//GPIO I2C Function
void GPIO_I2C_Init(void); 					// Make the SCL and SDA High to initialize the I2C
void GPIO_I2C_Start(void); 					// Generate a Start Flag in I2C
void GPIO_I2C_Stop(void);					// Generate a Stop Flag in I2C
void GPIO_I2C_TX_Byte(unsigned char byte);   // Write one byte
unsigned char GPIO_I2C_RX_Byte(void);		// Read one byte
void GPIO_I2C_Write(unsigned char TASadr_w, unsigned char reg, unsigned char txdata); //I2C Write Function
unsigned char GPIO_I2C_Read(unsigned char TASadr_w,unsigned char reg);				  //I2C Read Function
void  GPIO_I2C_WriteMutiBytes(unsigned char TASadr_w,unsigned char reg, unsigned int length, unsigned char *txdata_array);	//I2C Multi-byte Write
//USCI I2C Definition
void USCI_I2C_Init(void);               //USCI I2C Initial
unsigned char USCI_I2C_ReadOneByte(unsigned char slave_address, unsigned char register_address);                 		//USCI I2C Read
void USCI_I2C_WriteOneByte(unsigned char slave_address, unsigned char register_address, unsigned char register_value);	//USCI I2C Write
void  USCI_I2C_WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *Tx_array);//USCI I2C Multi-byte Write
//Function based on the I2C code. Which use the I2C function to configure the chip
void Audio_AppWrite(const reg_value *data, unsigned int length_data, unsigned char address);    						//Audio Chip Configuration Without Checking the Register
void Audio_RegValWriteDSP(const reg_value *data, unsigned int length_data, unsigned char address);                      //DSP Data Write
void Audio_AppWriteDSP(const reg_value *data,
					const reg_value *DSP_A_data,
					const reg_value *DSP_D_data,
					unsigned int length_data,
					unsigned int length_DSP_A,
					unsigned int length_DSP_D,
					unsigned char address);																				//Audio Chip Configuration With checking the Register 254 and 255

void AIC_Init(unsigned char address);      																			    //AIC Part Initial
void NonAIC_Init(const reg_value *data, unsigned int length_data, unsigned char address);							    //Non-AIC Part Initial

void TXString(char* string, int length); 	//Print Message in the UART
void Delay_ms(unsigned int time);			//Delay
void newlineonuart();                       //Newline in UART

/********************************************************************************************/
/*                    Main                                                                  */
/********************************************************************************************/
void main(void) {

	  WDTCTL = WDTPW + WDTHOLD;		  	// Stop WDT
	  I2C_METHOD = GPIO;
	  P1REN |= 0x08;                            // P1.3 pullup
	  P1IE |= 0x08;                             // P1.3 interrupt enabled
	  P1IES |= 0x08;                            // P1.3 Hi/lo edge
	  P1IFG &= ~0x08;                           // P1.3 IFG cleared
	// If the I2C_METHOD is 1, the code will use the GPIO I2C mode. And if the I2C I2C_METHOD IS NOT 1. The code will use the USCI I2C.
	  	  if(I2C_METHOD == 1)
	  {
		  BCSCTL1 = CALBC1_1MHZ;
		  DCOCTL = CALDCO_1MHZ;
		  GPIO_I2C_Init();
		  Delay_ms(10);
		  AIC_Init(AIC3254);
		  //NonAIC_Init(Tas5717ConfigureFile,sizeof(Tas5717ConfigureFile)/2,Tas5717);
		  Delay_ms(20);
		//UART Initial
			  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RX pin, P1.2=TX pin
			  P1SEL2 = BIT1 + BIT2 ;                    // P1SEL and P1SEL2 = 11--- Secondary peripheral module function is selected.
			  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
			  UCA0BR0 = 104;                            // 1MHz 9600
			  UCA0BR1 = 0;                              // 1MHz 9600
			  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
			  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
			  P1DIR |= 0x01;                            // LED On
			  P1OUT |= 0x01;
			  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
	  }
	  else
	  {
		  BCSCTL1 = CALBC1_8MHZ;
		  DCOCTL = CALDCO_8MHZ;
		  USCI_I2C_Init();
		  AIC_Init(AIC3254);
		  //NonAIC_Init(Tas5717ConfigureFile,sizeof(Tas5717ConfigureFile)/2,Tas5717);
		  P1DIR |= 0x01;                            // LED On
		  P1OUT |= 0x01;
	  }

}
// -----------------------------------------------------------------------------
// Interrupt Vector for Button
// -----------------------------------------------------------------------------
// Description:
//			The interrupt service is used in button press. When the button is pressed, it will go into this interrupt.
// -----------------------------------------------------------------------------
#pragma vector=PORT1_VECTOR
  __interrupt void Port_1(void)
{
  unsigned char P1IN_TMP;
  unsigned char i;
  P1IN_TMP = P1IN;
  for (i=0;i<50;i++)
  {
    if (P1IN_TMP == P1IN)
    	Delay_ms(2);
    else

      break;
  }
 if (i == 50)
 {
    switch (P1IFG)
   {
    case Button1_KEY_FG:             //Button 1 Pressed
    	if(Button1_State == 0x01)
    	Audio_AppWriteDSP(							// Load audio program
    						REG_Section_program1,		// Configuration
    						miniDSP_A_reg_values1,		// miniDSP_A C-RAM and I-RAM
    						miniDSP_D_reg_values1,		// miniDSP_D C-RAM and I-RAM
    						sizeof(REG_Section_program1)/2,
    						sizeof(miniDSP_A_reg_values1)/2,
    						sizeof(miniDSP_D_reg_values1)/2,
    						AIC3254);
    	else
        Audio_AppWriteDSP(							// Load audio program
        					REG_Section_program,		// Configuration
        					miniDSP_A_reg_values,		// miniDSP_A C-RAM and I-RAM
        					miniDSP_D_reg_values,		// miniDSP_D C-RAM and I-RAM
        					sizeof(REG_Section_program)/2,
        					sizeof(miniDSP_A_reg_values)/2,
        					sizeof(miniDSP_D_reg_values)/2,
        					AIC3254);

		 P1DIR ^= 0x01;
         break;

   }
    P1IFG = 0x00;
    Button1_State ^= 0x01;
  }
}
// -----------------------------------------------------------------------------
// Interrupt Vector for UART
// -----------------------------------------------------------------------------
// Description:
//			The interrupt service is used in UART. When we use the UART program, it will go into this interrupt.
//			When go into this interrupt, the code will check the first letter, r means read, w means write. And check the last letter whether it's "return"
//			Because the data from UART is ASCII code, we need to change the ASCII code to real data.
//			After change the ASCII to the real data, the code will I2C to modify the register.
//          Function:
//			1. Modify the register value on-line with UART code
//			Example: w301005
//			2. Read register value on-line with UART code
//          Example: r300301
// -----------------------------------------------------------------------------
#pragma vector=USCIAB0RX_VECTOR
 __interrupt void USCI0RX_ISR(void)
	  {
	    while (!(IFG2&UCA0RXIFG));                          // USCI_A0 TX buffer ready?
	    currcommand[currcommandbitpoint] = UCA0RXBUF;       // TX -> RXed character
	    switch(currcommandbitpoint)
	    	  	{
	    	  	case 7:    //Makes sure the buffer is full before processing it. (otherwise, moves the pointer +1)

	    	  			currcommandbitpoint = 0;			//Resets the buffer point. Not needed while processing.
	    	  			switch(currcommand[7])				//will happen based on the 7th bit of the current command (looking for "enter")
	    	  			{
	    	  				case 0x0D:						// Checks if the last character is Enter (carriage ret in Ascii is 0x0D)
	    	  				newlineonuart();
	    	  				TXString(Tx_message0,sizeof(Tx_message0));
	    	  				newlineonuart();
	    	  					// Insert the next switch and case here:
	    	  						// Need to test the first character of the command.
	    	  					   switch(currcommand[0])
	    	  						{
	    	  						case 'w': //check for little w
	    	  							newlineonuart();
	    	  							TXString(Tx_message1,sizeof(Tx_message1));
	    	  							newlineonuart();
	    	  							ADDR_Device = (currcommand[1]-48)*16+(currcommand[2]-48);  //Device Address
	    	  							ADDR_Register = (currcommand[3]-48)*16+(currcommand[4]-48);//Register Address
	    	  							ADDR_Value = (currcommand[5]-48)*16+(currcommand[6]-48);   //Data value
	    	  							Delay_ms(10);
	    	  							GPIO_I2C_Write(ADDR_Device, ADDR_Register, ADDR_Value);
	    	  							Delay_ms(10);
	    	  						break;
	    	  						case 'r': //check for little r
	    	  							newlineonuart();
	    	  							TXString(Tx_message2,sizeof(Tx_message2));
	    	  							newlineonuart();
	    	  							ADDR_Device = (currcommand[1]-48)*16+(currcommand[2]-48);  //Device Address
	    	  							ADDR_Register = (currcommand[3]-48)*16+(currcommand[4]-48);//Register Address
	    	  							ADDR_Value = (currcommand[5]-48)*16+(currcommand[6]-48);   //Data length
	    	  							Delay_ms(10);
	    	  							byte = GPIO_I2C_Read(ADDR_Device, ADDR_Register);
	    	  							Buffer[0]= byte/16+48;
	    	  						    Buffer[1]=(byte%16)+48;
	    	  							Delay_ms(10);
	    	  							newlineonuart();
	    	  							TXString(Buffer, sizeof(Buffer));
	    	  							newlineonuart();
	    	  							break;

	    	  						default:
	    	  						break;
	    	  						}
	    	  				break;

	    	  				default:							//This is what happens if the last character is not Enter.
	    	  				newlineonuart();
	    	  				TXString(Tx_message4,sizeof(Tx_message4));
	    	  				newlineonuart();

	    	  				break;
	    	  			}
	    	  		break;
	    	  	default:

	    	  	currcommandbitpoint++;

	  // while (!(IFG2&UCA0TXIFG));
	    UCA0TXBUF = UCA0RXBUF;
	    	  	}

	  }

// -----------------------------------------------------------------------------
// TXString
// -----------------------------------------------------------------------------
// Description:
// 		This function used for Print Message in the UART
// Inputs:
//		char* string
//		int length
// Outputs:
// 		none
// -----------------------------------------------------------------------------
 void TXString(char* string, int length )
 {
     int pointer;
     for( pointer = 0; pointer < length; pointer++)
     {
         volatile int i;
         UCA0TXBUF = string[pointer];
         while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
     }
 }

// -----------------------------------------------------------------------------
// newlineonuart
// -----------------------------------------------------------------------------
// Description:
// 		This function used for Newline in UART
// Inputs:
//		none
// Outputs:
// 		none
// -----------------------------------------------------------------------------
 void newlineonuart()
 {
	UCA0TXBUF = 0x0d;
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = 0x0a;
	while (!(IFG2&UCA0TXIFG));
  }

// -----------------------------------------------------------------------------
// Delay_ms
// -----------------------------------------------------------------------------
// Description:
// 		This function used for delay.
// Inputs:
//		unsigned int time
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void Delay_ms(unsigned int time)                     // Delay function in ms
{
  for(i=0; i<time; i++)
  {
    for(h=0; h<420; h++){}
  }
}

/********************************************************************************************/
/*    USCI I2C Code                                                                         */
/********************************************************************************************/
// -----------------------------------------------------------------------------
// USCI_I2C_Init
// -----------------------------------------------------------------------------
// Description:
// 		This function initializes the USCI module for master-transmit operation.
// Inputs:
//		none
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void USCI_I2C_Init(void)
{
  P1SEL |= SDA_PIN + SCL_PIN;						// Assign I2C pins to USCI_B0
  P1SEL2|= SDA_PIN + SCL_PIN;
  UCB0CTL1 = UCSWRST;								// Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;				// I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;					// Use SMCLK, keep SW reset
  UCB0BR0 = 12;										// Set I2C master speed
  UCB0BR1 = 0;										// Set I2C master speed
  UCB0CTL1 &= ~UCSWRST;								// Clear SW reset, resume operation
}

// -----------------------------------------------------------------------------
// USCI_I2C_WriteMultiBytes
// -----------------------------------------------------------------------------
// Description:
// 		This function is used to write multi bytes to I2C slaver.
// Inputs:
//		unsigned char slaver_address			=>	i2c address of slaver
//		unsigned char register_address			=>	slaver register address to be written
//		unsigned char write_length				=>	how many bytes to be written
//		unsigned char *Tx_array					=>	value array pointer to be written to slave
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void USCI_I2C_WriteMultiBytes(unsigned char slave_address, unsigned char register_address, unsigned char write_length, unsigned char *Tx_array)
{

  while (UCB0STAT & UCBBUSY);

  Tx_ByteCounter = write_length;

  Tx_Pointer_Temp = Tx_array;

  UCB0I2CSA = slave_address;

  UCB0CTL1 |= UCTR + UCTXSTT;                	 // I2C TX, start condition

  UCB0TXBUF = register_address; 				  //transferring register_address

  while (UCB0CTL1 & UCTXSTT);					 // waiting for slaver address was transferred

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
  UCB0CTL1 |= UCTXSTP;                    // I2C stop condition

  IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
}

// -----------------------------------------------------------------------------
// USCI_I2C_WriteOneByte
// -----------------------------------------------------------------------------
// Description:
// 		This function is used to write one byte to I2C slaver.
// Inputs:
// 		unsigned char slaver_address  		=>  i2c address of slaver
//		unsigned char register_address		=>  slaver register address to be written
//		unsigned char register_value     	=>  value to be written to slave
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void USCI_I2C_WriteOneByte(unsigned char slave_address, unsigned char register_address, unsigned char register_value)
{

  while (UCB0STAT & UCBBUSY);

  UCB0I2CSA = slave_address;

  UCB0CTL1 |= UCTR + UCTXSTT;                                   // I2C TX, start condition

  UCB0TXBUF = register_address;					//transferring register_address

  while (UCB0CTL1 & UCTXSTT);					 // waiting for slaver address was transferred

  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);

  UCB0TXBUF = register_value;					//transferring one byte value

  while((IFG2 & UCB0TXIFG) != UCB0TXIFG);

  UCB0CTL1 |= UCTXSTP;                    // I2C stop condition

  IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag

}

// -----------------------------------------------------------------------------
// USCI_I2C_ReadOneByte
// -----------------------------------------------------------------------------
// Description:
// 		This function is used to read one byte to I2C slaver.
// Inputs:
// 		unsigned char slaver_address  		=>  i2c address of slaver
//		unsigned char register_address		=>  slaver register address to be read
// Outputs:
//		unsigned char register_value     	=>  value to be read from slave
// -----------------------------------------------------------------------------
unsigned char USCI_I2C_ReadOneByte(unsigned char slave_address, unsigned char register_address)
{

  unsigned char Rx_Buffer;

  while (UCB0STAT & UCBBUSY);

  UCB0I2CSA = slave_address;

  UCB0CTL1 |= UCTR + UCTXSTT;                               // I2C TX, start condition

  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);

  UCB0TXBUF = register_address;					//transferring register_address

  while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);

  UCB0CTL1 &= ~UCTR;

  UCB0CTL1 |= UCTXSTT;                                   // I2C RX, restart condition

  while (UCB0CTL1 & UCTXSTT);                         // Loop until I2C STT is sent

  UCB0CTL1 |= UCTXSTP;

  while ((IFG2 & UCB0RXIFG) != UCB0RXIFG);

  Rx_Buffer = UCB0RXBUF;                 // Move RX data to Rx_Data

  IFG2 &= ~UCB0TXIFG;

  return(Rx_Buffer);

}

/********************************************************************************************/
/*    GPIO I2C Code                                                                         */
/********************************************************************************************/
// -----------------------------------------------------------------------------
// GPIO_I2C_Init
// -----------------------------------------------------------------------------
// Description:
// 		This function initializes the I2C's SCL and SDA.
// Inputs:
//		none
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void GPIO_I2C_Init(void)
{
	// Initialize the port
	// Set output to low
	OUTP &= ~(SDA | SCL);
	// Set direction of SDA and SCL to input to drive it high through pullup
	DIR &= ~(SDA | SCL);
}

// -----------------------------------------------------------------------------
// GPIO_I2C_Start
// -----------------------------------------------------------------------------
// Description:
// 		This function generate the I2C start flag
// Inputs:
//		none
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void GPIO_I2C_Start(void)
{
	// Set direction of SDA and SCL to input to drive it high through pullup
	DIR &= ~(SDA | SCL);
	DIR |= SDA;		// START condition. Drive SDA low while SCL high
	DIR |= SCL;             // Then drive SCL low
}

// -----------------------------------------------------------------------------
// GPIO_I2C_Stop
// -----------------------------------------------------------------------------
// Description:
// 		This function generate the I2C stop flag
// Inputs:
//		none
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void GPIO_I2C_Stop(void)
{
	DIR &= ~SCL;
	DIR &= ~SDA;
}

// -----------------------------------------------------------------------------
// GPIO_I2C_TX_Byte
// -----------------------------------------------------------------------------
// Description:
// 		Master send a byte via I2C to slave
// Inputs:
//		unsigned char byte
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void GPIO_I2C_TX_Byte(unsigned char byte)
{
    unsigned char n, mask = 0x80;

    for(n=0;n<8;n++)	                // Process 8 bits
        {
        if((byte & mask) != 0)	        // bit is high
            {
            DIR &= ~SDA;		// Set SDA

            //Make clock cycle
            DIR &= ~SCL;	    	// Set SCL
            DIR |= SCL;	    		// Reset SCL
            }
        else				// bit is low
            {
            DIR |= SDA;	    		// Reset SDA

            //Make clock cycle
            DIR &= ~SCL;	    	// Set SCL
            DIR |= SCL;	    		// Reset SCL
            }
        mask >>= 1;			// shift mask one bit right
        }
    // Acknowledge
    DIR &= ~SDA;			// Set SDA to input (high)
    n = 0;
    while((IN & SDA)!= 0)               // Wait until I2C device pulls SDA low
    {
	n++;
	if(n == 100)
	break;
    }
    // Last clock cycle
    OUTP |= SCL;				// Set SCL
    OUTP &= ~SCL;		        // Reset SCL
    DIR |= SDA; 		        // Set SDA to output (low)
    return;
}

// -----------------------------------------------------------------------------
// GPIO_I2C_RX_Byte
// -----------------------------------------------------------------------------
// Description:
// 		Master receive a byte via I2C from slave
// Inputs:
//		none
// Outputs:
// 		unsigned char byte
// -----------------------------------------------------------------------------
unsigned char GPIO_I2C_RX_Byte(void)
{
    unsigned char byte = 0x00;
    unsigned char mask;

    DIR &= ~SDA;

     for(mask=0x80;mask;mask >>=1)	                // Process 8 bits
        {
        if((IN & SDA ) != 0)	        // bit is high
            {
           	    		//
            byte |= mask;		//
            //Make clock cycle
            DIR &= ~SCL;	    	// Set SCL
            DIR |= SCL;	    		// Reset SCL

            }
        else				// bit is low
            {

            //Make clock cycle
            DIR &= ~SCL;	    	// Set SCL
            DIR |= SCL;	    		// Reset SCL

            }
       // byte <<= 1;			// shift byte one bit right
        }
    // Acknowledge
    DIR &= ~SDA;			// Set SDA to input (high)
    OUTP |= SCL;			// Set SCL
    DIR |= SDA; 		        // Set SDA to output (low)
    OUTP &= ~SCL;		        // Reset SCL
    DIR &= ~SDA;			// Set SDA to input (high)
    return byte;
}

// -----------------------------------------------------------------------------
// GPIO_I2C_Write
// -----------------------------------------------------------------------------
// Description:
// 		I2C_Write - send reg byte + one data byte
// Inputs:
//		unsigned char TASadr_w
//		unsigned char reg
//		unsigned char txdata
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void  GPIO_I2C_Write(unsigned char TASadr_w, unsigned char reg, unsigned char txdata)
{
    GPIO_I2C_Start();                        //Start I2C transaction
    GPIO_I2C_TX_Byte(TASadr_w);              //TAS5713 U2 address
    GPIO_I2C_TX_Byte(reg);                   //U2 register address
    GPIO_I2C_TX_Byte(txdata);                //Data
    GPIO_I2C_Stop();                         //Stop I2C transaction
}

// -----------------------------------------------------------------------------
// GPIO_I2C_WriteMutiBytes
// -----------------------------------------------------------------------------
// Description:
//		I2C_WriteMutiBytes - send reg byte + multiple data
// Inputs:
//		unsigned char TASadr_w
//		unsigned char reg
//		unsigned int length
//		unsigned char *txdata_array
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void  GPIO_I2C_WriteMutiBytes(unsigned char TASadr_w, unsigned char reg, unsigned int length, unsigned char *txdata_array)
{
	unsigned int i;
    GPIO_I2C_Start();                       //Start I2C transaction
    GPIO_I2C_TX_Byte(TASadr_w);             //TAS5713 U2 address
    GPIO_I2C_TX_Byte(reg);                  //U2 register address
    for(i=0;i<length;i++)
    {
        GPIO_I2C_TX_Byte(txdata_array[i]);  //Data
    }
    GPIO_I2C_Stop();                        //Stop I2C transaction
}

// -----------------------------------------------------------------------------
// GPIO_I2C_Read
// -----------------------------------------------------------------------------
// Description:
//		I2C_Read - send reg byte(read register) + one data byte to read the data
// Inputs:
//		unsigned char TASadr_w
//		unsigned char reg
// Outputs:
// 		unsigned char byte
// -----------------------------------------------------------------------------
unsigned char GPIO_I2C_Read(unsigned char TASadr_w, unsigned char reg)
{
    unsigned char byte = 0x00;
    unsigned char address =0x00;
    GPIO_I2C_Start();                       //Start I2C transaction
    address = TASadr_w | 0x01;
    GPIO_I2C_TX_Byte(TASadr_w);             //TAS5713 U2 address
    GPIO_I2C_TX_Byte(reg);                  //U2 register address
    GPIO_I2C_Stop();
    GPIO_I2C_Start();                       //Start I2C transaction
    GPIO_I2C_TX_Byte(address);              //TAS5713 U2 address
    byte = GPIO_I2C_RX_Byte();              //Data
    GPIO_I2C_Stop();                        //Stop I2C transaction
    return byte;
}

// -----------------------------------------------------------------------------
// Audio_RegValWriteDSP
// -----------------------------------------------------------------------------
// Description:
// 		Similar to Audio_AppWriteDSP but ignores miniDSP magic numbers
// Inputs:
//		const reg_value *data
//		unsigned int length_data
//		unsigned char address
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void Audio_RegValWriteDSP(const reg_value *data, unsigned int length_data, unsigned char address)
{
    int i;
    if(I2C_METHOD ==1)
    {
	for(i=0; i<length_data; i++)
	{
		switch(data[i].reg_off)
		{
		case 254:									// Delay case
			Delay_ms(data[i].reg_val);
		break;

		case 255:									// Ignore this case
		break;

		default:
			GPIO_I2C_Write(address,data[i].reg_off,data[i].reg_val);
		}
	}
    }
    else
    {
    	for(i=0; i<length_data; i++)
    	{
    		switch(data[i].reg_off)
    		{
    		case 254:									// Delay case
    			Delay_ms(data[i].reg_val);
    		break;

    		case 255:									// Ignore this case
    		break;

    		default:
    			USCI_I2C_WriteOneByte(address,data[i].reg_off,data[i].reg_val);
    		}
    	}
    }
}
// -----------------------------------------------------------------------------
// Audio_AppWrite
// -----------------------------------------------------------------------------
// Description:
// 		Parses a reg_value structure (typically  used for a non-miniDSP application).
// Inputs:
//		const reg_value *data
//		unsigned int length_data
//		unsigned char address
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void Audio_AppWrite(const reg_value *data, unsigned int length_data, unsigned char address)
{
    int i;
    if(I2C_METHOD == 1)
    {
    	for(i=0; i<length_data; i++)
    	{
    		GPIO_I2C_Write(address,data[i].reg_off,data[i].reg_val);
    	}
    }
    else
    {
    	for(i=0; i<length_data; i++)
    	{
    		USCI_I2C_WriteOneByte(address,data[i].reg_off,data[i].reg_val);
    	}
    }
}
// -----------------------------------------------------------------------------
// Audio_AppWriteDSP
// -----------------------------------------------------------------------------
// Description:
// 		Parses a reg_value structure (typically used for a miniDSP application).
//		The reg_off magic number 254 calls a delay subroutine. A 255 calls
//		audio_regvalWrite to load:
//			- DSP_A_data (if reg_val = 0x00)
//			- DSP_D_data (if reg_val = 0x01)
//		This function also automatically determines if registers are contiguous
//		to perform auto-incremented writes.
// Inputs:
//		const reg_value *data
//		const reg_value *DSP_A_data
//		const reg_value *DSP_D_data
//		unsigned int length_data
//		unsigned int length_DSP_A
//		unsigned int length_DSP_D
//		unsigned char address
// Outputs:
// 		none
// -----------------------------------------------------------------------------
void Audio_AppWriteDSP(const reg_value *data,
					const reg_value *DSP_A_data,
					const reg_value *DSP_D_data,
					unsigned int length_data,
					unsigned int length_DSP_A,
					unsigned int length_DSP_D,
					unsigned char address)
{
	int i;
	if(I2C_METHOD == 1)
	{
		for(i=0; i<length_data; i++)
		{
			switch(data[i].reg_off)
			{
			case 254:									// Delay case
				Delay_ms(data[i].reg_val);
				break;

			case 255:									// miniDSP code case
				if(data[i].reg_val == 0x00)
				{
					Audio_RegValWriteDSP(DSP_A_data,
								 length_DSP_A, address);
				}
				else
				{
					if(data[i].reg_val == 0x01)
						Audio_RegValWriteDSP(DSP_D_data,
									 length_DSP_D, address);
				}
				break;

			default:
				GPIO_I2C_Write(address,data[i].reg_off,data[i].reg_val);
			}
		}
	}
	else
	{
		for(i=0; i<length_data; i++)
				{
					switch(data[i].reg_off)
					{
					case 254:									// Delay case
						Delay_ms(data[i].reg_val);
						break;

					case 255:									// miniDSP code case
						if(data[i].reg_val == 0x00)
						{
							Audio_RegValWriteDSP(DSP_A_data,
										 length_DSP_A, address);
						}
						else
						{
							if(data[i].reg_val == 0x01)
								Audio_RegValWriteDSP(DSP_D_data,
											 length_DSP_D, address);
						}
						break;

					default:
						USCI_I2C_WriteOneByte(address,data[i].reg_off,data[i].reg_val);
					}
				}
	}
}

// -----------------------------------------------------------------------------
// AIC_Init
// -----------------------------------------------------------------------------
// Description:
// 		Parses a reg_value structure (typically used for a miniDSP application).
// Inputs:
//		const reg_value *data
//		const reg_value *DSP_A_data
//		const reg_value *DSP_D_data
//		unsigned int length_data
//		unsigned int length_DSP_A
//		unsigned int length_DSP_D
//		unsigned char address
// Outputs:
// 		none
void AIC_Init(unsigned char address)
{
	Audio_AppWriteDSP(							// Load audio program
					REG_Section_program,		// Configuration
					miniDSP_A_reg_values,		// miniDSP_A C-RAM and I-RAM
					miniDSP_D_reg_values,		// miniDSP_D C-RAM and I-RAM
					sizeof(REG_Section_program)/2,
					sizeof(miniDSP_A_reg_values)/2,
					sizeof(miniDSP_D_reg_values)/2,
					address);
}

// -----------------------------------------------------------------------------
// NonAIC_Init
// -----------------------------------------------------------------------------
// Description:
// 		Parses a reg_value structure (typically used for a non-miniDSP application).
// Inputs:
//		const reg_value *data
//		unsigned int length_data
//		unsigned char address
// Outputs:
// 		none
void NonAIC_Init(const reg_value *data, unsigned int length_data, unsigned char address)
{
	int i,j=0;
	unsigned char TXdata[30];
	if(I2C_METHOD == 1)
	{
		for(i=0; i<length_data; i++)
		{
			if ((data[i].reg_off == data[i+1].reg_off)& (i!= length_data-1))
			{
				TXdata[j] = data[i].reg_val;
				j++;
			}
			else if ((data[i].reg_off != data[i+1].reg_off)& (j!= 0))
			{
				TXdata[j] = data[i].reg_val;
				GPIO_I2C_WriteMutiBytes(address,data[i].reg_off,j+1,TXdata);
				j = 0;
			}
			else
				GPIO_I2C_Write(address,data[i].reg_off,data[i].reg_val);
		}
	}
			else
			{
				for(i=0; i<length_data; i++)
				{
					if ((data[i].reg_off == data[i+1].reg_off)& (i!= length_data-1))
					{
						TXdata[j] = data[i].reg_val;
						j++;
					}
					else if ((data[i].reg_off != data[i+1].reg_off)& (j!= 0))
					{
						TXdata[j] = data[i].reg_val;
						USCI_I2C_WriteMultiBytes(address,data[i].reg_off,j+1,TXdata);
						j = 0;
					}
					else
						USCI_I2C_WriteOneByte(address,data[i].reg_off,data[i].reg_val);
					}
			}
}
