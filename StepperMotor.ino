// Kai Hiratani
// May 9th, 2023

#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 100

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 9, 10, 11, 12);

// the previous reading from the analog input
int previous = 0;

void setup() {
  // set the speed of the motor to 15 RPMs
  stepper.setSpeed(15);
}

void loop() {
  // get the sensor value
  int val = adc_read(0);

  // move a number of steps equal to the change in the
  // sensor reading
  stepper.step(val - previous);

  // remember the previous value of the sensor
  previous = val;
}

//---------------------------------------------------------------------------------------------------------------------------------------

/*
#include <Stepper.h>
#include <avr/io.h>
#define BUTTON_PIN 4 //pin 26 PA4


// module Global variables
float Temp_Threshold = 77;
float Water_Threshold = 250;

// Global stepper varialbs
const int stepsPerRevolution = 2048;  // number of steps for a full revolution
const int rolePerMinute = 10;  // rotations per minute
Stepper myStepper(stepsPerRevolution, 9, 11, 10, 12);  // initialize the stepper library on pins 9 through 12:
const int steps = stepsPerRevolution / 360;
int buttonState = HIGH; // Flag to keep track of the button state

// Function prototypes
void Vent_control();
//void ERROR_STATE();

void setup() {
  // Enable pull-up resistor for the button pin
  PORTB |= (1 << BUTTON_PIN);
  
  // initializing the serial port
  Serial.begin(9600);

  // Stepper motor
  myStepper.setSpeed(rolePerMinute);  // Initiating stepper motor
  
}

void loop()
{
  Vent_control();
  /*if (*pin_k == B00000000)  //on/off == off
    {
      Serial.println("DISABLED State");

      DisabledState();
    }

  if (*pin_k == B00000001 && Water_level() > Water_Threshold && DHT_sensor() < Temp_Threshold) //on/off == on && water level > threshold && temp < threshold
    {
      Serial.println("IDLE State");

      IdleState();
    }

  if (*pin_k == B00000001 && Water_level() > Water_Threshold && DHT_sensor() >= Temp_Threshold) //on/off == on && water level > threshold && temp > threshold || reset == on
    {
      Serial.println("RUNNING State");

      RunningState();
    }

  if (*pin_k == B00000001 && Water_level() < Water_Threshold) //on/off == on && water level < threshold
    {
      Serial.println("ERROR State");

      ERROR_STATE();
    }
  */
}

/*
// Should be located in Idle, Running, and Error States.
void Vent_control() 
{
  int newButtonState = PIND & (1 << BUTTON_PIN);

  // If the button state has changed
  if (newButtonState != buttonState) {
    // Update the button state
    buttonState = newButtonState;

    // If the button is pressed
    if (buttonState == LOW) {
      // Print "1" the first time the button is pressed
      static bool isFirstPress = true;
      if (isFirstPress) {
        // step one revolution  in one direction:
        myStepper.step(steps);
        Serial.println ("left");
      }
      // Print "2" on the next press
      else {
        // step one revolution in the other direction:
        myStepper.step(-steps);
        Serial.println ("right");
      }
    }
  }
}
*/

/*
void ERROR_STATE()
{
  // Turn the RED LED on

  // reset != pressed && water level < threshold
  while ( && Water_level() < Water_Threshold)    
  {
    // Call lcd to display error state
    Vent_control();
  }
    // Turn the LED off

    // Clearing the lcd screen before leaving ERROR State
}
*/


//--------------------------------------------------------------------------------------------------------------------------------
/*
#include <Stepper.h>

// Global stepper variables
const int stepsPerRevolution = 2048;  // number of steps for a full revolution
const int rolePerMinute = 10;  // rotations per minute
Stepper myStepper(stepsPerRevolution, 9, 11, 10, 12);  // initialize the stepper library on pins 9 through 12:
const int steps = stepsPerRevolution / 360;


// B register
// volatile unsigned char* port_b = (unsigned char) 0x25; // Setting the port_b (data register) to address 0x25 (sets bit as high or low, outputs data)
// volatile unsigned char ddr_b = (unsigned char) 0x24;  // Setting the ddr_b (Data Direction Register) to address 0x24 (sets it as input or output)
// volatile unsigned char pin_b = (unsigned char) 0x23;  // Setting pin_b (Input Pin Address) to 0x23 (Reading a value from a pin)


// K register
volatile unsigned char pin_k = (unsigned char) 0x106;
volatile unsigned char ddr_k = (unsigned char) 0x107;
volatile unsigned char port_k = (unsigned char*) 0x108;

// Function Prototypes
void Vent_control();

void setup() {
  // initializing the serial port
  Serial.begin(9600);

  // button input
  ddr_k = B01111111;

  // Stepper motor
  myStepper.setSpeed(rolePerMinute);  // Initiating stepper motor
}

void loop() {

  // The Vent_control() function should be placed inside the Idle, Running, and Error States.
  Vent_control();

}

void Vent_control() 
{
  int steps = stepsPerRevolution / 360;
  //Serial.println ("waiting for direction");

    while(pin_k == B00000010) //Analog pin 9
    {
      // step one revolution  in one direction:
      myStepper.step(steps);
      Serial.println ("left");
    }

    while(*pin_k == B00000100) //Analog pin 10
    {
      // step one revolution in the other direction:
      myStepper.step(-steps);
      Serial.println ("right");
    }
}
*/
