//Assignment: CPE301_Final_Project
//Produced by: Kai Hiratani

//pin initialization:
//ADC
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

//UART 
#define RDA 0x80
#define TBE 0x20  
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;  //UCSRnA holds flags set or reset when transmission or reception is complete
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;  //enabling transmit and receive using bits 3 and 4
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;  //set session characteristics, bits 7:6 set UART mode, bits 2:1 set character length 
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;  //used to set baud rate, 12 bits are used, 4 bit reserved
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;  //used to send and receive data

#define WRITE_HIGH(pin_num) *port_a |= (0x01 << pin_num);
#define WRITE_LOW(pin_num) *port_a &= ~(0x01 << pin_num);

volatile unsigned char* my_PCICR = (unsigned char*) 0x68;
volatile unsigned char* my_PCMSK0 = (unsigned char*) 0x6B;
volatile unsigned char* my_PCIFR = (unsigned char*) 0x3B;
volatile unsigned char* my_EIMSK = (unsigned char*) 0x3D;
volatile unsigned char* my_EICRA = (unsigned char*) 0x69;
volatile unsigned char* my_EICRB = (unsigned char*) 0x6A;
volatile unsigned char* my_EIFR = (unsigned char*) 0x3C;

volatile unsigned char* port_a= (unsigned char*) 0x22;
volatile unsigned char* ddr_a= (unsigned char*) 0x21;
volatile unsigned char* pin_a= (unsigned char*) 0x20;

// Define Port B Register Pointers
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;
volatile unsigned char* pin_b  = (unsigned char*) 0x23; 

//PORT K setup (PK7) A15 for water level sensor 
volatile unsigned char* port_k = (unsigned char*) 0x108; 
volatile unsigned char* ddr_k  = (unsigned char*) 0x107; 
volatile unsigned char* pin_k  = (unsigned char*) 0x106; 


// Define Port D Register Pointers
volatile unsigned char* port_d = (unsigned char*) 0x2B; 
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A; 
volatile unsigned char* pin_d  = (unsigned char*) 0x29; 


//Allowed libraries
#include <LiquidCrystal.h>
#include <DHT.h>
#include <RTClib.h>
#include <Stepper.h>

#define THRESHOLD 100

//Stepper motor from data sheet
#define STEPS 2038

//stepper
#define BUTTON_PIN 4 //pin 26 PA4

//sets pin and chooses device
DHT dht(6, DHT11);

// Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(8, 7, 5, 4, 3, 2);

const int stepsPerRevolution = 2048;  // number of steps for a full revolution
const int rolePerMinute = 10;  // rotations per minute
Stepper myStepper(stepsPerRevolution, 9, 11, 10, 12);  // initialize the stepper library on pins 9 through 12:
int prev = 0;
const int steps = stepsPerRevolution / 360;
int buttonState = HIGH; // Flag to keep track of the button state

//RTC 
RTC_DS1307 clock;

//defining var
char curState;
float conductivity; 
float waterLevel; 
//int waterLevelPin = A15; 
//Motor Variables
bool fan_prev_state;
bool fan_now_state = false;


//-----------------------------------------------Begin functions--------------------------------------------------
void setup() {
    //function prototypes? 
    //UART setup
    U0init(9600); 

    // setup the ADC
    adc_init();

    //myStepper setup speed
    myStepper.setSpeed(15);

    //sets LCD's columns and rows
    lcd.begin(16, 2);
    dht.begin();
    curState = 'i';

    *ddr_a &= 0b00001111;
    *port_a |= 0b11110000;
    
    //set motor(ddr) to OUTPUT and enable pin(port) to LOW 
    *ddr_b |= 0b10000000;
    *port_b &= 0b01111111;

    *ddr_k &= 0b01111111;

    ISR_config();
}

void loop() {
    waterLevel = (waterLevelVoltage()- 0.5)*100; 

    int val = adc_read(0);

    // move # of steps equal to the change in the sensor reading
    myStepper.step(val - prev);

    prev = val;
    stateSel();
}

void adc_init()
{
    // setup the A register
    *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
    *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
    *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
    *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
    // setup the B register
    *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
    *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
    // setup the MUX Register
    *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
    *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
    *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
    *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num)
{
    // clear the channel selection bits (MUX 4:0)
    *my_ADMUX  &= 0b11100000;
    // clear the channel selection bits (MUX 5)
    *my_ADCSRB &= 0b11110111;
    // set the channel number
    if(adc_channel_num > 7)
    {
        // set the channel selection bits, but remove the most significant bit (bit 3)
        adc_channel_num -= 8;
        // set MUX bit 5
        *my_ADCSRB |= 0b00010000;
    }
    // set the channel selection bits
    *my_ADMUX  += adc_channel_num;
    // set bit 6 of ADCSRA to 1 to start a conversion
    *my_ADCSRA |= 0x40;
    // wait for the conversion to complete
    while((*my_ADCSRA & 0x40) != 0);
    // return the result in the ADC data register
    return *my_ADC_DATA;
}

void U0init(unsigned long U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
//
// Read USART0 RDA status bit and return non-zero true if set
//
unsigned char U0kbhit()
{
  //Check the RDA status bit, and return True if the bit is set(1), return False if the bit
  //is clear(0).
  return (RDA & *myUCSR0A); 
}
//
// Read input character from USART0 input buffer
//
unsigned char U0getchar()
{
  return *myUDR0;
}

void U0putchar(unsigned char U0pdata)
{
  while(!(*myUCSR0A & TBE)){};
  *myUDR0 = U0pdata;
}


void lcd_d(){
    //reads the temp and humidity from the dht device
    float hum = dht.readHumidity();
    float temp= dht.readTemperature();

    //convert to fahrenheit because dht reads in as celsius
    float fah = ( temp * 1.8 ) + 32;

    //sets cursor to col 1 and row 1
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    // where the temp value goes
    lcd.print(fah);
    lcd.write(223);
    lcd.print("F");

    //sets cursor to col 2 and row 1
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    // where the humidity value goes
    lcd.print(hum);
    lcd.print("%");

    if(fah > 76){
        curState = 'r';
        changeState();
    }
    if(fah <= 76 && curState != 'i'){
        curState = 'i';
        changeState();
    }
  
}

void running(){
  //turn on blue and everything else off
  WRITE_HIGH(2);
  WRITE_LOW(0);
  WRITE_LOW(1);
  WRITE_LOW(3);

    lcd_d();
    fan_control(true);

    if (waterLevel < THRESHOLD) {
        printStringToSerial("Alert: water level is too low");
        curState = 'e';
        error();
        changeState();
    }
}

void disabled(){
  //turn on yellow and everything else off
  WRITE_HIGH(3);
  WRITE_LOW(0);
  WRITE_LOW(1);
  WRITE_LOW(2);

    lcd.clear();
}

void idle(){
  //turn on green and everything else off
  WRITE_HIGH(0);
  WRITE_LOW(1);
  WRITE_LOW(2);
  WRITE_LOW(3);
    
    lcd_d();
    fan_control(false);

    if (waterLevel < THRESHOLD) {
        printStringToSerial("Alert: water level is too low");
        curState = 'e';
        error();
        changeState();
    }
}

void error(){
    //turn on red and everything else off
    WRITE_HIGH(1);
    WRITE_LOW(0);
    WRITE_LOW(2);
    WRITE_LOW(3);

    lcd.clear();
    lcd.print("ERROR");
    fan_control(false);

    if (waterLevel > THRESHOLD) {
        curState = 'i';
        changeState();
    }

    }

void stateSel(){
    if(curState == 'd'){
        disabled();
    }
    if(curState == 'r'){
        running();
    }
    if(curState == 'i'){
        idle();
    }
    if(curState == 'e'){
        error();
    }
}

void changeState(){
    String state= "";
    if(curState == 'd'){
        state = "disabled";
    }
    if(curState == 'i'){
        state = "idle";
    }
    if(curState == 'e'){
        state = "error";
    }
    if(curState == 'r'){
        state = "running";
    }
    printStringToSerial("Changed to " + state + "state.");
}

ISR(PCINT0_vect)
{
  if(curState == 'd'){
    curState = 'i';
    changeState();
  }
  else{
    curState = 'd';
    changeState();
  }
}

void ISR_config() {
  *my_PCICR |= (1<<PCIE0);
  PCMSK0 |= (1<<PCINT0);
}

float waterLevelVoltage() {
    return (adc_read(pin_k))*5/1023; 
}

//print to Serial monitor using UART
void printStringToSerial(String inputString) 
{
    DateTime currTime = clock.now();
    String sPrint = String(currTime.year());

    inputString = inputString + "\n";
    
    for(int x = 0; x < inputString.length(); x++) {
        U0putchar(inputString[x]);
    }

    for(int i = 0; i < sPrint.length(); i++) {
      U0putchar(sPrint[i]);
    }
    U0putchar('-');
    sPrint = currTime.month();
    
    for(int j = 0; j < sPrint.length(); j++) {
      U0putchar(sPrint[j]);
    }
    U0putchar('-');
    sPrint = currTime.day();
    
    for(int k = 0; k < sPrint.length(); k++) {
      U0putchar(sPrint[k]);
    }
    U0putchar(' ');
    sPrint = currTime.hour();
    
    for(int l = 0; l < sPrint.length(); l++) {
      U0putchar(sPrint[l]);
    }
    U0putchar(':');
    sPrint = currTime.minute();
    
    for(int m = 0; m < sPrint.length(); m++) {
      U0putchar(sPrint[m]);
    }
    U0putchar(':');
    sPrint = currTime.second();
    
    for(int n = 0; n < sPrint.length(); n++) {
      U0putchar(sPrint[n]);
    }
}

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

