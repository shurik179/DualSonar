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



#define I2C_DEFAULT_ADDRESS 0x11;
#define I2C_STATUS_REG 0x01
#define I2C_DATA_REG 0x02


class DualSonar
{
    public:
      DualSonar(uint8_t I2C_ADDDRESS); 
      void start();
      void stop();
      bool update();
      bool isActive();
      uint16_t distanceL();
      uint16_t ditanceR();
    private:
      uint8_t _I2C_ADDRESS;  
      uint16_t _dLeft, _dRight;
}
#endif
