/*
 * GPIO_I2C.h
 *
 * Copyright 2015 Joseph Warner, Eric Bauer, Gus Fragasse
 */

#ifndef LOOPERPEDAL_LIBS_GPIO_I2C_H_
#define LOOPERPEDAL_LIBS_GPIO_I2C_H_

// C headers
#include <msp.h>
#include <stdint.h>
// C++ headers
#include "I2C.h"

namespace LooperPedal {

class GPIO_I2C : public I2C {
 public:
  // Deconstructor
  virtual ~GPIO_I2C();
  // Constructor
  GPIO_I2C(uint32_t i_OUTP,
           uint32_t i_DIR,
           uint32_t i_IN,
           uint16_t i_SDA,
           uint16_t i_SCL)
      : I2C() {
    // Set internal variables
    OUTP = i_OUTP;
    DIR = i_DIR;
    IN = i_IN;
    SDA = i_SDA;
    SCL = i_SCL;

    // Initialize the port
    // Set output to low
    HWREG8(OUTP) &= ~(SDA | SCL);
    // Set direction of SDA and SCL to input to drive it high through pullup
    HWREG8(DIR) &= ~(SDA | SCL);
  }

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
  /* =========== Methods =========== */
  // Generate a Start Flag in I2C
  void Start();
  // Generate a Stop Flag in I2C
  void Stop();
  // Write one byte
  void TX_Byte(unsigned char byte);
  // Read one byte
  unsigned char RX_Byte();

  /* =========== Members =========== */
  // Hardware registers
  // HWREG8(x)
  uint32_t OUTP;
  uint32_t DIR;
  uint32_t IN;
  // SDA and SCL pin numbers
  uint16_t SDA;
  uint16_t SCL;
};

} /* namespace LooperPedal */

#endif  // LOOPERPEDAL_LIBS_GPIO_I2C_H_
