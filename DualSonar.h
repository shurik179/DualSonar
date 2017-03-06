/*
 * DualSonar - library for using dual sonar sensor, 
 * consisting of two HC-SR04 and ATtiny85
 * See detailed info here: https://github.com/shurik179/DualSonar
 * Created by Alexander Kirillov <shurik179@gmail.com> on March 5, 2017
 * Version: 1.0, last updated on March 5, 2017
 */
#ifndef DualSonar_h
#define DualSonar_h

#include "Arduino.h"
#include <Wire.h>

#define I2C_STATUS_REG 0x01
#define I2C_DATA_REG 0x02


class DualSonar
{
    public:
      DualSonar(byte i2c_address); 
      void begin(void);
      void stop(void);
      bool update(void);
      bool isActive(void);
      uint16_t distanceL(void);
      uint16_t distanceR(void);
    private:
      byte _i2c_address;  
      uint16_t _dLeft;
      uint16_t _dRight;
};
#endif
