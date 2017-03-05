/*
  Example of DualSonar library. This library allows one to create an I2C dual sonar sensor
  out of two HC-SR04 ultrasonic sensors and ATtiny85 (or Adafruit Trinket) microcontroller,
  which provides the I2C interface.
  To use this library:
  1. Upload the script DualSonar-slave.ino, found in extras subfolder of this library,
     to ATtiny85 or Trinket. Make sure it runs at 8 or 16 Mhz. This sketch requires TinyWireS library
  2. Connect two HC-SR04 sensors to ATtiny85as follows: 
     - sensor 1 (left): Trig pin to ATtiny85 #1, Echo pin to #3
     - sensor 2 (right): Trig pin to ATtiny85 #1, Echo pin to #4
  3. Connect ATtiny85 to your Arduino as follows:
     Ground to Ground, 
     ATtiny85 power to Arduino 5V  
     ATtiny85 #0 to Arduino SDA (on Uno, it is pin A4)  
     ATtiny85 #2 to Arduino SCL (on Uno, it is pin A5)
     for other Arduinos, see https://www.arduino.cc/en/Reference/Wire
  4. Upload this sketch to Arduino
  5. Start serial monitor at 9600 Baud
*/
#include <Wire.h>
#include <DualSonar.h>


/*
 Create new sonar object.
 Optionally, you can also provide I2C address of the sonar:
 mySonar(0x13);
 In this case, make sure the I2C address provided matches the I2C address in the I2C slave sketch
 running on ATtiny85 MCU 
*/
DualSonar mySonar(); 

void setup()
{
    Serial.begin(9600);
    Serial.println("Dual Sonar Test"); Serial.println("");
    //start pinging
    mySonar.start();
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
       r : start sensor
       s : stop sensor
    */
    char c=' ';
    while (Serial.available())
    {
        c=Serial.read();
    }
    if (c=='r')
    {
        mySonar.start();
        Serial.println("Sonar restarted");
    } else if (c=='s')
    {
        mySonar.stop();
        Serial.println("Sonar stopped");
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


