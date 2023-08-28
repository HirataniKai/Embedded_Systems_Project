//Code by Kai Hiratani
//May 7, 2023

//DC Motor Fan: fan turns on/off when temperature falls out of the specified range (high or low), no libraries allowed 

// Define Port B Register Pointers
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;
volatile unsigned char* pin_b  = (unsigned char*) 0x23; 

//Motor Variables
bool fan_prev_state;
bool fan_now_state = false;
//int mSpeed = 255; //motor speed
//int speedPin = 13;
//pin 13 PB7 set bit 7 on, 0b10000000 0x80
int motor_pin = 7; 
//pin 27, PA5

//set motor(ddr) to OUTPUT and enable pin(port) to LOW 
*ddr_b |= 0b10000000;
*port_b &= 0b01111111;

void fan_control(bool on){
  if(on == true){
    //digitalWrite(speedPin, HIGH)
    *port_b |= 0b10000000; //turn fan motor    
    fan_now_state = true;   //current fan state set to true
  }
  else if(on == false){
    //digitalWrite(speedPin, LOW)
    *port_b &= 0b01111111; //turn fan motor off
    fan_now_state = false;   //current fan state set to false
  }
  if(fan_now_state != fan_prev_state){
    printStringToSerial("Fan has changed state.");  //when fan changes state, record 
  }
  fan_prev_state = fan_now_state;
}
