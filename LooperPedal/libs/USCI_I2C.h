/*
 * USCI_I2C.h
 *
 * Copyright 2015 Joseph Warner, Eric Bauer, Gus Fragasse
 */

#ifndef LOOPERPEDAL_LIBS_USCI_I2C_H_
#define LOOPERPEDAL_LIBS_USCI_I2C_H_

// Class header
#include "I2C.h"
// C headers
#include <stdint.h>
#include <msp.h>

namespace LooperPedal {

class USCI_I2C : public I2C {
 public:
  // Deconstructor
  virtual ~USCI_I2C();
  // Constructor
  USCI_I2C(uint16_t i_SDA_PIN, uint16_t i_SCL_PIN);
  // I2C Write Function
  void Write(unsigned char slave_address,
             unsigned char register_address,
             unsigned char txdata);
  // I2C Multi-byte Write
  void WriteMultiBytes(unsigned char slave_address,
                       unsigned char register_address,
                       unsigned char write_length,
                       unsigned char* tx_array);
  // I2C Read Function
  unsigned char Read(unsigned char slave_address,
                     unsigned char register_address);

 private:
  // SDA and SCL pin numbers
  uint16_t SDA_PIN;
  uint16_t SCL_PIN;
};

} /* namespace LooperPedal */

#endif  // LOOPERPEDAL_LIBS_USCI_I2C_H_
