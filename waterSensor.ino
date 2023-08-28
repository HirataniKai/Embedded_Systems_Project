//Code by Kai Hiratani
//May 6, 2023

//Monitor the water levels in a reservoir and print an alert when the level is too low

//Do not use library functions such as pinMode, etc. You may use the predefined macros for registers and pin positions.

//Threshold detection can use either an interrupt from the comparator or via a sample using the ADC.
//– You may NOT use the ADC library to perform the sampling

//global water level sensor variables 
int value; 
float waterLevel; 
int waterLevelPin = A15; 
#define THRESHOLD 250

 //ADC
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

//PORT K setup (PK7) A15
volatile unsigned char* port_k = (unsigned char*) 0x108; 
volatile unsigned char* ddr_k  = (unsigned char*) 0x107; 
volatile unsigned char* pin_k  = (unsigned char*) 0x106; 

//UART as well
#define RDA 0x80
#define TBE 0x20  
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;  //UCSRnA holds flags set or reset when transmission or reception is complete
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;  //enabling transmit and receive using bits 3 and 4
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;  //set session characteristics, bits 7:6 set UART mode, bits 2:1 set character length 
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;  //used to set baud rate, 12 bits are used, 4 bit reserved
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;  //used to send and receive data

void setup() {
    // setup water level sensor
    *ddr_c &= 0b01111111;

    //UART setup
    U0init(9600);

    // setup the ADC
    adc_init();

}

void loop() {
    float conductivity = adc_read(pin_k); 
    waterLevel = (conductivity - 0.5)*100; 

    if (waterLevel < THRESHOLD) {
        Serial.println("Alert: water level is too low");

    } else if(waterLevel > THRESHOLD) {
        Serial.println("Alert: water leak detected");
    }
}

//in main as well 
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

//in main as well 
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


//in main: 
float waterLevelgetter() {
    return adc_read(waterLevelPin); 
}

void printStringToSerial(String inputString) 
{
    DateTime currTime = clock.now();
    String sPrint = String(currTime.year());

    inputString = inputString + "\n";
    
    for(int x = 0; x < inputString.length(); x++) {
        U0putChar(inputString[x]);
    }

    for(int i = 0; i < sPrint.length(); i++) {
      U0putchar(sPrint[i]);
    }
    U0putChar('-');
    sPrint = currTime.month();
    
    for(int j = 0; j < sPrint.length(); j++) {
      U0putChar(sPrint[j]);
    }
    U0putChar('-');
    sPrint = currTime.day();
    
    for(int k = 0; k < sPrint.length(); k++) {
      U0putChar(sPrint[k]);
    }
    U0putChar(' ');
    sPrint = currTime.hour();
    
    for(int l = 0; l < sPrint.length(); l++) {
      U0putChar(sPrint[l]);
    }
    U0putChar(':');
    sPrint = currTime.minute();
    
    for(int m = 0; m < sPrint.length(); m++) {
      U0putChar(sPrint[m]);
    }
    U0putChar(':');
    sPrint = currTime.second();
    
    for(int n = 0; n < sPrint.length(); n++) {
      U0putChar(sPrint[n]);
    }
}