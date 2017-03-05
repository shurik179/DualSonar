# DualSonar
Dual sonar I2C sensor for Arduino, using two HC-SR04 ultrasonic sensors and ATtiny85 MCU (or Adafruit Trinket).
## Rationale
HC-SR04 are cheap and reliable ultrasonic sensors, used in many robotics projects with Arduino or similar microcontrollers. However, they have two drawbacks:

 1. The standard method of reading the sensor in Arduino, using `PulseIn()` function, ties up the microcontroller for up to 30ms and interferes with tasks which need to be run frequently such as WiFi communication using Blynk or reading input from other sensors over serial interface.

 2. Each sensor requires 2 pins, so if you have several ultrasonic senors, you run out of pins quickly.

There are some ways to mitigate these probems, such as using interrupts instead of `PulseIn` or using NewPing library which allows one to use only one pin per sensor. This project goes one step further. Namely, we combine two HC-SR04 sensors and an ATtiny85 microcontroller to create a sensor which measures distance in two directions and communicates with Arduino or another MCU using I2C protocol, all under $10. 
You can combine several such dual sonar sensors, putting them on the same I2C bus (see below), so you can have virtually unlimited number of sonars using only 2 pins of Arduino.
## Materials
- 2 HC-SR04 sensors (can be bought from a variety of sources on [Amazon](https://www.amazon.com/s/ref=nb_sb_noss_2?url=search-alias%3Daps&field-keywords=hc-sr04), eBay, AlieExpress for as sheap as $2/piece)
- ATtiny85 MCU ($1.24 on [DigiKey](https://www.digikey.com/product-detail/en/microchip-technology/ATTINY85-20PU/ATTINY85-20PU-ND/735469). 
- jumper wires, headers, small pcb

To program the ATtiny, you can use a special programmer such as [SparkFun Tiny AVR Programmer](https://www.sparkfun.com/products/11801) or an Arduino Uno as described [here](https://www.hackster.io/arjun/programming-attiny85-with-arduino-uno-afb829). Alternatively, you can replace ATtiny85 by [Adafruit Trinket](https://www.adafruit.com/product/1501) (5V version). It is more expensive ($6.95), but comes with a microUSB port, so you do not need a separate programmer. If you have never used microcontrollers other than Arduino, we recommend using Trinket; Adafruit has a great tutorial on using Trinket.

These materials are enough for one DualSonar sensor, which can be connected to an Arduino or any microcontroller which can act as I2C master (e.g., ESP8266-12).

## Programming ATtiny85
The first thing you must do is upload the Arduino sketch `DualSonar-slave.ino`, included in `extras` folder of this repository, to ATtiny85 or Trinket. Open the sketch in Arduino IDE and upload it to ATtiny85 or Trinket. If you are using ATtiny85 rather than Trinket, also configure it to run at 8Mhz.  
  This sketch requires TinyWireS library, available [here](https://github.com/rambo/TinyWire).

## Connections
Connect the ATtiny85/Trinket to the HC-SR04's as follows:
- Sensor1 (left): Trig pin <-> ATtiny85 pin 1

    Echo pin <->ATtiny85 pin 3
- Sensor2 (right): Trig pin <-> ATtiny85 pin 1
    Echo pin <->ATtiny85 pin 4
In addition, connect GND pin of each sensor to GND of ATtiny85/Trinket, and Vcc pin of each sensor to Vcc pin of ATtiny85 (or 5V on Trinket). 

The sensor is done.

## Using the DualSonar sensor
To use the sensor, install `DualSonar` library (of which this file is a part). It provides the following functions


