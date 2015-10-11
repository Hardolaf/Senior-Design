/*
 * Audio_AIC.cpp
 *
 *  Created on: Oct 11, 2015
 *      Author: Eric Bauer
 */
 
Audio_AIC::~Audio_AIC(){
	// TODO Deconstructor
}

Audio_AIC::Audio_AIC(unsigned char address, GPIO_I2C i_i2c_manager){
	// Initialization for GPIO managed I2C master
	
}

Audio_AIC::Audio_AIC(unsigned char address, USCI_I2C i_i2c_manager){
	// Initialization for USCI managed I2C master
	
}


void Audio_AppWriteDSP(const reg_value *data,
				const reg_value *DSP_A_data,
				const reg_value *DSP_D_data,
				unsigned int length_data,
				unsigned int length_DSP_A,
				unsigned int length_DSP_D,
				unsigned char address){
					
					
}

void Audio_RegValWriteDSP(const reg_value *data, unsigned int length_data, unsigned char address){
	
	
}