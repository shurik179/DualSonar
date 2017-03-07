#include <avr/interrupt.h>
#include <TinyWireS.h>
#define REG_STATUS 0x01 //I2C register 1: holds status (1 byte)
#define REG_DATA 0x02   //I2C register 2: holds data (4 bytes: 
                        // distance 1 - high, d1- low, d2-high, d2-low                               
#define I2C_SLAVE_ADDRESS 0x11 //I just like number 17
#define ECHO1_PIN 3 //if you change these values, also change the enabled interrupts in setup()
#define ECHO2_PIN 4 
#define TRIG_PIN 1 
#define TIMEOUT 20000 //timeout waiting for return echo, in microseconds. 20000 is about 3.44 meters
#define INTERVAL 20   //interval between conclusion of one ping and beginning of next, in ms
uint32_t ping_emit_time = 0,           //in microseconds
         last_ping_time = 0;           //time of completion of last ping, in milliseconds
volatile uint32_t echo_start_time = 0; //in microseconds
volatile uint16_t echo_duration = 0;   // duration of returned pulse in microseconds
volatile  bool  echo_pin_state;        //last known state of echo pin - for detecting changes

uint8_t echo_pin[2]={ECHO1_PIN, ECHO2_PIN};
uint16_t  distance[2] = {0,0};       //measured distance, in mm. Running average of last measurements,
                                      //using simple low-pass filter

volatile bool status=false; // is the sensor active?
uint8_t active_sensor=0;      //which of the two sensors are currently using? Possible values:0,1.
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
  echo_duration=0;
  echo_pin_state = LOW;  // must be low, we do not even need to check
}

void end_ping()
{
  //update distances
  // rule: distance =0.7* distance+0.3* new_distance;
  // new distance = echo_duration/5.8 (in mm)
  distance[active_sensor] = 0.7 * distance[active_sensor] + 3 * echo_duration / 58;
  pinging = false;
  last_ping_time = millis();
  //switch active sensor
  active_sensor=1-active_sensor;
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
    //detect pin state changes for echo pin
    if ((echo_pin_state == LOW) && (digitalRead(echo_pin[active_sensor]))) //transition low->high
    {
      echo_start_time = micros();
      echo_pin_state = HIGH;
    } else if ((echo_pin_state == HIGH ) && (digitalRead(echo_pin[active_sensor]) == LOW) ) //high->low
    {
      echo_duration = micros() - echo_start_time;
      echo_pin_state = LOW;
    }
    

  }//end of 'if pinging'
}


void setup()
{
  pinMode(ECHO1_PIN, INPUT);
  pinMode(ECHO2_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN,LOW);
  delay(100);
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
    if (( micros() - ping_emit_time > TIMEOUT ) || (micros() < ping_emit_time ) )  //detect rollover
    {
      if (!echo_duration)
        echo_duration = TIMEOUT;
    }
    // check if we are done
    if ( echo_duration  )
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
      TinyWireS.send(highByte(distance[0]));
      TinyWireS.send(lowByte(distance[0]));
      TinyWireS.send(highByte(distance[1]));
      TinyWireS.send(lowByte(distance[1]));
      break;
    default:
      break;
      //send nothing
  }
}

