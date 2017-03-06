/*
  DualSonar - library for using dual sonar sensor, 
  consisting of two HC-SR04 and ATtiny85
  See detailed info here: https://github.com/shurik179/DualSonar
  Created by Alexander Kirillov <shurik179@gmail.com> on March 5, 2017
  Version: 1.0, last updated on March 5, 2017
 
   To use this example file:
   1. Put together and program the ATtiny85 chip and two HC-SR04 sensors as desribed here:
  https://github.com/shurik179/DualSonar
   2.Connect ATtiny85 to your Arduino as follows:
     Ground to Ground, 
     ATtiny85 power to Arduino 5V  
     ATtiny85 #0 to Arduino SDA (on Uno, it is pin A4)  
     ATtiny85 #2 to Arduino SCL (on Uno, it is pin A5)
     for other Arduinos, see https://www.arduino.cc/en/Reference/Wire
  3. Upload this sketch to Arduino
  4. Start serial monitor at 9600 Baud
*/
#include <Wire.h>
#include <DualSonar.h>


/*
 Create new sonar object. The argument must be the I2C address of the sensor.
 Default address is 0x11 (or 17, in decimal). It can be changed by editing 
 DualSonar-slave.ino sketch before uploading it to ATtiny85.
*/
DualSonar mySonar(0x11); 

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    Serial.println("Dual Sonar Test"); Serial.println("");
    //start pinging
    mySonar.begin();
    //check sensor status
    if (mySonar.isActive())   
    {   //all ok
        Serial.println("Sonar started");
        Serial.println("Type 's' to stop the sensor, 'r' to restart");
        delay(500);
    } else 
    {
        Serial.println("Problem starting the sonar. Please check connections.");        
    }

}


void loop()
{   
    /*
      Let the user enter commands at serial monitor
      Available commands (each is one char)
       r : restart sensor
       s : stop sensor
    */
    char c=' ';
    if (Serial.available())
    {
        c=Serial.read();
    }

    if (c=='r')
    {
        mySonar.begin();
        Serial.println("Sonar restarted. Enter 's' to stop");
    } else if (c=='s')
    {
        mySonar.stop();
        Serial.println("Sonar stopped. Enter 'r' to restart");
    }        
    if (mySonar.isActive()) {
        //update sensor readings
        mySonar.update();
        /*Print the left and right distances received at last update
          Values are in mm. Maximal distance is about 3444 mm (3.444 m);
          if the distance is larger than that, or if no echo was received, 
          the value of 3444 is returned.    
        */
        Serial.print("Left distance (mm): ");
        Serial.print(mySonar.distanceL());
        Serial.print("  Right distance (mm): ");
        Serial.println(mySonar.distanceR());
        //delay - no point in printing the values too frequently
        //note that the sonar will continue to be taking measurements continually
        //but will only send these values to Arduino when you run update() function        
        delay(300);
    }
}


