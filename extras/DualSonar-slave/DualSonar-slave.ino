#include <avr/interrupt.h>
#include <TinyWireS.h>
#define REG_STATUS 0x01 //I2C register 1: holds status (1 byte)
#define REG_DATA 0x02   //I2C register 2: holds data (4 bytes: 
                        // distance 1 - high, d1- low, d2-high, d2-low                               
#define I2C_SLAVE_ADDRESS 0x11 //I just like number 17
#define ECHO1_PIN 3 //if you change these values, also change the enabled interrupts in setup()
#define ECHO2_PIN 4 
#define TRIG_PIN 1 
#define TIMEOUT 20000 //timeout waiting for return echo, in microseconds. 20000 is about 3.5 meters
#define INTERVAL 50   //interval between conclusion of one ping and beginning of next, in ms
uint32_t ping_emit_time = 0,                  //in microseconds
         last_ping_time = 0;                 //time of competion of last ping, in milliseconds
volatile uint32_t echo1_start_time = 0, echo2_start_time = 0; //in microseconds
volatile uint16_t echo1_duration = 0, echo2_duration = 0; //in microseconds
volatile  bool  echo1_pin_state, echo2_pin_state; //last known state of pins - for detecting changes

uint16_t  distance1 = 0, distance2 = 0;       //measured distance, in mm. Running average of last measurements,
//using simple low-pass filter

volatile bool status=false; // is the sensor active?
volatile uint8_t requestedRegister = 0; //requested i2c register
bool  pinging=false;                               //are we actively pinging now?

void start_ping()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  ping_emit_time = micros();
  pinging = true;
  echo1_pin_state = LOW; echo2_pin_state = LOW; //they must be low, we do not even need to check
}

void end_ping()
{
  //update distances
  // rule: distance =0.7* distance+0.3* new_distance;
  // new distance = echo_duration/5.8 (in mm)
  if (echo1_duration > 100) //shorter durations are most likely result of error in readings
    distance1 = 0.7 * distance1 + 3 * echo1_duration / 58;
  if (echo2_duration > 100)
    distance2 = 0.7 * distance2 + 3 * echo2_duration / 58;
  //reset variables
  pinging = false;
  last_ping_time = millis();
  echo1_duration = 0; echo2_duration = 0;
  //digitalWrite(LED_PIN,LOW);
}

// Interrupt vector for external interrupt on pin PCINT7..0
// This will be called when any of the pins D0 - D4 on the trinket change
// or pins D8 - D13 on an Arduino Uno change.

// the echo pin will flip HIGH at the point when the pulse has completed
// and timing should begin, it will then flip LOW once the sound wave is received
// so we need to detect both of these states

ISR(PCINT0_vect)
{
  if (pinging)
  {
    //detect pin state changes for echo1
    if ((echo1_pin_state == LOW) && (digitalRead(ECHO1_PIN))) //transition low->high
    {
      echo1_start_time = micros();
      echo1_duration = 0;
      echo1_pin_state = HIGH;
    } else if ((echo1_pin_state == HIGH ) && (digitalRead(ECHO1_PIN) == LOW) ) //high->low
    {
      echo1_duration = micros() - echo1_start_time;
      echo1_pin_state = LOW;
    }
    //detect pin state changes for echo2
    if ((echo2_pin_state == LOW) && (digitalRead(ECHO2_PIN))) //transition low->high
    {
      echo2_start_time = micros();
      echo2_duration = 0;
      echo2_pin_state = HIGH;
    } else if ((echo2_pin_state == HIGH ) && (digitalRead(ECHO2_PIN) == LOW) ) //high->low
    {
      echo2_duration = micros() - echo2_start_time;
      echo2_pin_state = LOW;
    }

  }//end of 'if pinging'
}


void setup()
{
  pinMode(ECHO1_PIN, INPUT);
  pinMode(ECHO2_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  //enable interrupts - see https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
  GIMSK |= 0b00100000;    // turns on pin change interrupts
  PCMSK |= 0b00011000;    // turn on interrupts on pin PB3, PB4
  sei();
  //start I2C
  TinyWireS.begin(I2C_SLAVE_ADDRESS); // join i2c
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);
}

void loop()
{ 
  TinyWireS_stop_check(); //check i2c bus
  if (pinging)
  {
    //check timeouts for echos
    if ( micros() - ping_emit_time > TIMEOUT )
    {
      if (!echo1_duration)
        echo1_duration = TIMEOUT;
      if (!echo2_duration)
        echo2_duration = TIMEOUT;
    }
    // check if we are done
    if ( echo1_duration && echo2_duration )
      end_ping();

  } 
  else //not pinging
  {
    if  ( status && (millis() - last_ping_time > INTERVAL))
      start_ping();
  }
}

void receiveEvent(uint8_t howMany) //number of bytes received
{
  if (howMany == 0) return; //no data sent
  //first byte must be register address
  requestedRegister = TinyWireS.receive();
  if ((howMany == 2) && (requestedRegister == REG_STATUS))
  {
    //this is a write request to REG_STATUS
    status = (bool)TinyWireS.receive();
  }
  //in all other cases, ignore subsequent bytes (if any)

}

void requestEvent() //send previously requested register
{

  switch (requestedRegister)
  {
    case 1: //requested status
      TinyWireS.send(status);
      break;
    case 2: //requested data
      //send four bytes      
      TinyWireS.send(highByte(distance1));
      TinyWireS.send(lowByte(distance1));
      TinyWireS.send(highByte(distance2));
      TinyWireS.send(lowByte(distance2));
      break;
    default:
      break;
      //send nothing
  }
}

