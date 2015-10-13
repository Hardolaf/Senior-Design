/*
 * Audio_AIC.cpp
 *
 *  Created on: Oct 11, 2015
 *      Author: Eric Bauer
 */

#include "type.h"
#include "base_main_Rate44_pps_driver.h"
#include "Audio_AIC.h"
#include "I2C.h"
#include "misc.h"
#include "msp.h"
#include <stdint.h>

namespace LooperPedal {

Audio_AIC::~Audio_AIC() {
	// TODO Deconstructor
}

Audio_AIC::Audio_AIC(unsigned char address, I2C &i_i2c_manager) {
	i2c_manager = i_i2c_manager;
	// Initialization of AIC device settings from Pure Path Studio File
	Audio_AppWriteDSP(							// Load audio program
					REG_Section_program,		// Configuration array generated from Pure Path Studio
					miniDSP_A_reg_values,		// miniDSP_A C-RAM and I-RAM
					miniDSP_D_reg_values,		// miniDSP_D C-RAM and I-RAM
					sizeof(REG_Section_program)/2,
					sizeof(miniDSP_A_reg_values)/2,
					sizeof(miniDSP_D_reg_values)/2,
					address);	
}


void Audio_AppWriteDSP(const reg_value *data,
				const reg_value *DSP_A_data,
				const reg_value *DSP_D_data,
				unsigned int length_data,
				unsigned int length_DSP_A,
				unsigned int length_DSP_D,
				unsigned char address){
					
		for(int i=0; i<length_data; i++) 
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
				i2c_manager.Write(address,data[i].reg_off,data[i].reg_val);
			}
		}
}

void Audio_RegValWriteDSP(const reg_value *data,
		unsigned int length_data,
		unsigned char address){
	for(int i=0; i<length_data; i++)
	{
		switch(data[i].reg_off)
		{
		case 254:									// Delay case
			Delay_ms(data[i].reg_val);
		break;

		case 255:									// Ignore this case
		break;

		default:
			i2c_manager.Write(address,data[i].reg_off,data[i].reg_val);
		}
	}	
}

} /* namespace LooperPedal */
