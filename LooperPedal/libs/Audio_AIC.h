/*
 * Audio_AIC.h
 *
 *  Created on: Oct 11, 2015
 *      Author: Eric Bauer
 */

#ifndef LIBS_Audio_AIC_H_
#define LIBS_Audio_AIC_H_

#include "I2C.h"

namespace LooperPedal {

class Audio_AIC {
public:
	// Deconstructor
	virtual ~Audio_AIC();
	// Constructor & Initialization
	Audio_AIC(unsigned char address, I2C &i_i2c_manager);
private: 
	// Audio Chip Configuration With checking the Register 254 and 255
	void Audio_AppWriteDSP(const reg_value *data,
					const reg_value *DSP_A_data,
					const reg_value *DSP_D_data,
					unsigned int length_data,
					unsigned int length_DSP_A,
					unsigned int length_DSP_D,
					unsigned char address);
	// DSP Data Write
	void Audio_RegValWriteDSP(const reg_value *data,
			unsigned int length_data,
			unsigned char address);
	I2C &i2c_manager;
};

} /* namespace LooperPedal */

#endif /* LIBS_I2C_H_ */
