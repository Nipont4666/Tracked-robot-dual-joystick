/*
 2.4G Transmitter nRF24L01 module Arduino
 Joystick control dual motors via L298N
 Written by: Pason Tanpaiboon
 May 2016
 Version.1.3
  
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

uint8_t data[5] ; 
const uint8_t buffer_size = sizeof(data);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };//two pipes communication 

///////////////////////Joystick Left///////////////////////////////////////
const int potInPinLX = A0;// X axis input
const int potInPinLY = A1;//Y axis input

int potInPinLXval = 0; 
int potInPinLYval = 0;       
////////////////////////Joystick Right//////////////////////////////////////
const int potInPinRX = A2;// X axis input
const int potInPinRY = A3;//Y axis input

int potInPinRXval = 0; 
int potInPinRYval = 0;  
////////////////////////Potentiometer///////////////////////////////////////
const int potInPinM = A4;
int potInPinMval = 0; 

void setup(void)
{
   Serial.begin(115200);
   pinMode(potInPinLX,INPUT );
   pinMode(potInPinLY,INPUT );
   pinMode(potInPinRX,INPUT );
   pinMode(potInPinRY,INPUT );
   pinMode(potInPinM,INPUT );
     
  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.stopListening();
}

void loop(void)
{
 ////////////////////////////////////////////////////////////////////////   
 potInPinLXval = analogRead(potInPinLX); 
 delay(15);
 data[0] = potInPinLXval/10;//scaling
 //////////////////////////////////////////////////////////////////////// 
 potInPinLYval = analogRead(potInPinLY); 
 delay(15);
 data[1] = potInPinLYval/10;//scaling 
 //////////////////////////////////////////////////////////////////////// 
 potInPinRXval = analogRead(potInPinRX); 
 delay(15);
 data[2] = potInPinRXval/10;//scaling
 //////////////////////////////////////////////////////////////////////// 
 potInPinRYval = analogRead(potInPinRY); 
 delay(15);
 data[3] = potInPinRYval/10;//scaling 
 //////////////////////////////////////////////////////////////////////// 
 potInPinMval = analogRead(potInPinM); 
 delay(15);
 data[4] = potInPinMval/10;//scaling  
 /////////////////////////////////////////////////////////////////////////

  radio.stopListening();
  bool ok = radio.write(  data ,sizeof(data) );
  delay(100);
  radio.startListening();
  delay(30);
  if (ok){
  Serial.print("data[0]=");
  Serial.print(data[0]);
  Serial.print(" data[1]=");
  Serial.print(data[1]);
  Serial.print(" data[2]=");
  Serial.print(data[2]);
  Serial.print(" data[3]=");
  Serial.print(data[3]);
  Serial.print(" data[4]=");
  Serial.println(data[4]);
  }
  else {
   
  Serial.println ("Failed");
 
  }
 
}//void loop()
 
 
 
    

  
















