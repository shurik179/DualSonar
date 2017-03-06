#include "Arduino.h"
#include <Wire.h>
#include "DualSonar.h"
//constructor
DualSonar::DualSonar(byte i2c_address)
{
    _i2c_address=i2c_address;
}
void DualSonar::begin()
{
    Wire.beginTransmission(_i2c_address);
    Wire.write((byte)I2C_STATUS_REG);
    Wire.write((byte)1);
    Wire.endTransmission();
}
void DualSonar::stop()
{
    Wire.beginTransmission(_i2c_address);
    Wire.write((byte)I2C_STATUS_REG);
    Wire.write((byte)0);
    Wire.endTransmission();
}
bool DualSonar::update()
{
    byte data[4]; //high1, low1, high2, low2
    Wire.beginTransmission(_i2c_address);
    Wire.write((byte)I2C_DATA_REG);
    Wire.endTransmission();
   
    Wire.requestFrom(_i2c_address, (byte)4);    // request 4 bytes from slave device
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
    Wire.beginTransmission(_i2c_address);
    Wire.write((byte)I2C_STATUS_REG);
    Wire.endTransmission();
   
    Wire.requestFrom(_i2c_address, (byte)1);    // request 1 byte from slave device
    if (Wire.available() && (bool)Wire.read())
    { 
       return true;
    }        
    else return false;
}

uint16_t DualSonar::distanceL()
{
    return _dLeft;    
}

uint16_t DualSonar::distanceR()
{
    return _dRight;    
}
