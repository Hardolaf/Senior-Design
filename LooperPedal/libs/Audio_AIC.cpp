/*
 * Audio_AIC.cpp
 *
 * Copyright 2015 Joseph Warner, Eric Bauer, Gus Fragasse
 */

// Class Header
#include "Audio_AIC.h"
// C headers
#include <stdint.h>
#include <msp.h>
// Project Headers
#include "./type.h"
#include "./I2C.h"
#include "./misc.h"
// Auto generated header
#include "base_main_Rate44_pps_driver.h"

namespace LooperPedal {

Audio_AIC::~Audio_AIC() {
  //  Deconstructor
}

void Audio_AIC::Audio_AppWriteDSP(const reg_value* data,
                                  const reg_value* DSP_A_data,
                                  const reg_value* DSP_D_data,
                                  unsigned int length_data,
                                  unsigned int length_DSP_A,
                                  unsigned int length_DSP_D,
                                  unsigned char address) {
  for (int i = 0; i < length_data; i++) {
    switch (data[i].reg_off) {
      case 254:  // Delay case
        Delay_ms(data[i].reg_val);
        break;

      case 255:  // miniDSP code case
        if (data[i].reg_val == 0x00) {
          Audio_RegValWriteDSP(DSP_A_data, length_DSP_A, address);
        } else {
          if (data[i].reg_val == 0x01)
            Audio_RegValWriteDSP(DSP_D_data, length_DSP_D, address);
        }
        break;

      default:
        i2c_manager.Write(address, data[i].reg_off, data[i].reg_val);
    }
  }
}

void Audio_AIC::Audio_RegValWriteDSP(const reg_value* data,
                                     unsigned int length_data,
                                     unsigned char address) {
  for (int i = 0; i < length_data; i++) {
    switch (data[i].reg_off) {
      case 254:  // Delay case
        Delay_ms(data[i].reg_val);
        break;

      case 255:  // Ignore this case
        break;

      default:
        i2c_manager.Write(address, data[i].reg_off, (data[i].reg_val));
    }
  }
}

} /* namespace LooperPedal */
