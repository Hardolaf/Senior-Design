/*
 * misc.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: warne_000
 */

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
  for(unsigned int i=0; i<time; i++)
  {
    for(int h=0; h<420; h++){}
  }
}


