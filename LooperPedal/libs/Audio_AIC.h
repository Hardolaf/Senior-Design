/*
 * Audio_AIC.h
 *
 * Copyright 2015 Joseph Warner, Eric Bauer, Gus Fragasse
 */

#ifndef LOOPERPEDAL_LIBS_AUDIO_AIC_H_
#define LOOPERPEDAL_LIBS_AUDIO_AIC_H_

// C headers
#include <stdint.h>
#include <msp.h>
// C++ Headers
#include "I2C.h"
#include "./type.h"
// Auto generated header
#include "base_main_Rate44_pps_driver.h"

namespace LooperPedal {

class Audio_AIC {
 public:
  // Deconstructor
  virtual ~Audio_AIC();
  // Constructor & Initialization
  Audio_AIC(unsigned char address, I2C* i_i2c_manager)
      : i2c_manager(*i_i2c_manager) {
    // Initialization of AIC device settings from Pure Path Studio File
    Audio_AppWriteDSP(
        // Load audio program
        REG_Section_program,   // Configuration array generated from Pure Path
                               // Studio
        miniDSP_A_reg_values,  // miniDSP_A C-RAM and I-RAM
        miniDSP_D_reg_values,  // miniDSP_D C-RAM and I-RAM
        sizeof(REG_Section_program) / 2, sizeof(miniDSP_A_reg_values) / 2,
        sizeof(miniDSP_D_reg_values) / 2, address);
  }

 private:
  // Audio Chip Configuration With checking the Register 254 and 255
  void Audio_AppWriteDSP(const reg_value* data,
                         const reg_value* DSP_A_data,
                         const reg_value* DSP_D_data,
                         unsigned int length_data,
                         unsigned int length_DSP_A,
                         unsigned int length_DSP_D,
                         unsigned char address);
  // DSP Data Write
  void Audio_RegValWriteDSP(const reg_value* data,
                            unsigned int length_data,
                            unsigned char address);
  I2C& i2c_manager;
};

} /* namespace LooperPedal */

#endif  // LOOPERPEDAL_LIBS_AUDIO_AIC_H_
