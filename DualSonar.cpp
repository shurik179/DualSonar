#include "Arduino.h"
#include <Wire.h>
#include "DualSonar.h"
//constructor
DualSonar::DualSonar(uint8_t I2C_ADDRESS=I2C_DEFAULT_ADDRESS)
{
    _I2C_ADDRESS=I2C_ADDRESS;
}
void DualSonar::start()
{
    Wire.beginTransmission(_I2C_ADDRESS);
    Wire.write((byte)I2C_STATUS_REG);
    Wire.write((byte)1);
    Wire.endTransmission();
}
void DualSonar::stop()
{
    Wire.beginTransmission(_I2C_ADDRESS);
    Wire.write((byte)I2C_STATUS_REG);
    Wire.write((byte)0);
    Wire.endTransmission();
}
bool DualSonar::update()
{
    byte data[4]; //high1, low1, high2, low2
    Wire.beginTransmission(_I2C_ADDRESS);
    Wire.write((byte)I2C_DATA_REG);
    Wire.endTransmission();
   
    Wire.requestFrom(_I2C_ADDRESS, 4);    // request 4 bytes from slave device
    int i=0;
    while (Wire.available()&& i<4) 
    {
        data[i]=Wire.read();
        i++;
    }
    if (i<4) 
    {
        return false; //failed to get data
    }
    _dLeft= uint16_t(data[0]) << 8 | data[1];
    _dRight= uint16_t(data[2]) << 8 | data[3];
    return true;
}
bool DualSonar::isActive()
{
    Wire.beginTransmission(_I2C_ADDRESS);
    Wire.write((byte)I2C_STATUS_REG);
    Wire.endTransmission();
   
    Wire.requestFrom(_I2C_ADDRESS, 1);    // request 1 byte from slave device
    if (Wire.available() && (bool)Wire.read())
    { 
       return true;
    }        
    else return false;
}

uint16_t distanceL()
{
    return _dLeft;    
}

uint16_t distanceR()
{
    return _dRight;    
}
