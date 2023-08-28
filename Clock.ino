//Code by Kai Hiratani
//May 7, 2023

//Clock: used for reporting events, clock library allowed

#include <RTClib.h>
//RTC(Real Time Clock) set up 
RTC_DS1307 clock;

// Define Port D Register Pointers
volatile unsigned char* port_d = (unsigned char*) 0x2B; 
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A; 
volatile unsigned char* pin_d  = (unsigned char*) 0x29; 

//print to Serial monitor using UART, operates as clock
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

/*
in main.ino but use UART 
void printCurrTime(){
currTime = rtc.now();
Serial.print(" (");
Serial.print(curr.year(), DEC);
Serial.print('-');
Serial.print(curr.month(), DEC);
Serial.print('-');
Serial.print(curr.day(), DEC);
Serial.print(") ");
Serial.print(curr.hour(), DEC);
Serial.print(':');
Serial.print(curr.minute(), DEC);
Serial.print(':');
Serial.println(curr.second(), DEC);
 }
*/



