/*

 2.4G Transmitter nRF24L01 module Arduino
 2 Channel Joystick Radio Control + L298N dual motor control for Tracked Robot
 Written by: Pason Tanpaiboon
 June 2016
 Version.1.2
   
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

*/
 
#include <Servo.h> 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };//2 pipes communication
uint8_t received_data[5];
uint8_t num_received_data = sizeof(received_data);

//Right Motor 
const int enableR = 3;
const int MotorR1 = 4;
const int MotorR2 = 5;

//Left Motor 
const int enableL = 6;
const int MotorL1 = 7;
const int MotorL2 = 8;

int enableRval ;
int enableLval ;
int MotorR1val ;
int MotorR2val ;
int MotorL1val ;
int MotorL2val ;

int MotorRspeed ;
int MotorLspeed ;

int Motorspeed = 80 ;// Variable motors speed Setup from 0 to 255
/////////////////////////////////////////////////////////
int LXaxis_Val ;
int LYaxis_Val ;

/////////////////////////////////////////////////////////

void setup(void)
{
 
  Serial.begin(115200);
 
  pinMode (enableR, OUTPUT);
  pinMode (MotorR1, OUTPUT);
  pinMode (MotorR2, OUTPUT);  

  pinMode (enableL, OUTPUT);
  pinMode (MotorL1, OUTPUT);
  pinMode (MotorL2, OUTPUT);  
  
  digitalWrite(enableR , HIGH);
  digitalWrite(enableL , HIGH);
////////////////////////////////////////////////////////////////////// 
  
  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();

}//void setup

void loop(void)
{
  
///////////////////////////////Radio reading///////////////////////////////////////////////////////     
  if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read(&received_data, num_received_data);
      delay(10);//increase delay for longer data
    }// while (!done)
 
    Serial.print (LXaxis_Val); 
    Serial.print ("---"); 
    Serial.print (LYaxis_Val); 
    Serial.print ("---"); 
     
    radio.stopListening();

    radio.startListening();

/**************************************************************Transmission error*********************************************************************/
if ((LXaxis_Val == 0 && LYaxis_Val == 0 )||(LXaxis_Val == 255 && LYaxis_Val == 255 )) // radio error
  {
      
      MotorRspeed = 0;
      MotorLspeed = 0;
 
      MotorR1val = 0;//off 
      MotorR2val = 0;
      MotorL1val = 0;
      MotorL2val = 0;
 
      Engine();
      Serial.print("Transmission error!");
  }
/**************************************************************Left X axis*********************************************************************/
    LXaxis_Val = received_data[0];
    delay(10);
    LXaxis_Val = LXaxis_Val*10;//scale back

/**************************************************************Left Y axis*********************************************************************/
    LYaxis_Val = received_data[1];
    delay(10);
    LYaxis_Val = LYaxis_Val *10;//scale back
    
/*************************************************************Stopping***************************************************************/ 

      if (LXaxis_Val >= 490  && LXaxis_Val <= 525 && LYaxis_Val >= 490  && LYaxis_Val <= 510 )//Y-stick nutral + X-stick neutral
     {
      
      MotorRspeed = 0;
      MotorLspeed = 0;
 
      MotorR1val = 0;//off 
      MotorR2val = 0;
      MotorL1val = 0;
      MotorL2val = 0;
 
      Engine();
      Serial.print("Stop");
     } 
  
/*************************************************************Straight Forward***************************************************************/   
   
     if (  LYaxis_Val >= 0   &&   LYaxis_Val < 490 && LXaxis_Val < 525  &&  LXaxis_Val > 490 )//Move Y-stick forward + X-stick neutral
     {
    //Neutral position 490 -- 520
     MotorRspeed =  map(LYaxis_Val,510,0,0,Motorspeed);

     MotorLspeed =  MotorRspeed;
       
      MotorR1val = 1;
      MotorR2val = 0;
      MotorL1val = 1;
      MotorL2val = 0;
      Engine();
      delay (20);
      Serial.print("Forward");
     }      

/*************************************************************Straight Backward***************************************************************/        
  
  if (   LYaxis_Val > 515 &&  LXaxis_Val <= 520   &&  LXaxis_Val >= 490  )//Move Y-stick backward + X-stick neutral    
   {
      
     MotorRspeed = map(LYaxis_Val,510,1020,0,Motorspeed);
         
     MotorLspeed = MotorRspeed;
        
      MotorR1val = 0;
      MotorR2val = 1;
      MotorL1val = 0;
      MotorL2val = 1;   
      Engine();
      Serial.print("Backward");
   }
       
 /********************************************************************Forward Right Direction***************************************************************/   
 
  if (LYaxis_Val < 490 && LXaxis_Val > 520 )//Move Y-stick forward + X-stick right    
     {
         
     MotorRspeed = 0;
           
     MotorLspeed = map(LYaxis_Val,0,510,Motorspeed,0);
      
      MotorR1val = 1;
      MotorR2val = 0;
      MotorL1val = 1;
      MotorL2val = 0;
      Engine();
      Serial.print("Forward Right");
     }
/********************************************************************Forward Left Direction***************************************************************/      
     
  if ( LYaxis_Val >= 0 &&  LYaxis_Val < 490 && LXaxis_Val >= 0 && LXaxis_Val < 490 )//Move Y-stick forward + X-stick left    
     {
          
     MotorRspeed = map(LYaxis_Val,0,500,Motorspeed,0);
     
     MotorLspeed = 0;
        
      MotorR1val = 1;
      MotorR2val = 0;
      MotorL1val = 1;
      MotorL2val = 0;   
      Engine();
      Serial.print("Forward Left");
     }
/********************************************************************Backward Right Direction***************************************************************/   
     
   if (LYaxis_Val > 510 && LXaxis_Val > 520 )//Move Y-stick forward + X-stick right    
     {
       
     MotorRspeed = 0;
          
     MotorLspeed = map(LYaxis_Val,510,1020,0,Motorspeed);
          
      MotorR1val = 0;
      MotorR2val = 1;
      MotorL1val = 0;
      MotorL2val = 1;
      Engine();
      Serial.print("Backward Right");
     }
     
/************************************************Backward Left Direction***************************************************************/   
   
  if (LYaxis_Val > 510 && LXaxis_Val < 490 && LXaxis_Val < 520)//Move Y-stick backward + X-stick left    
     {
         
     MotorRspeed = map(LYaxis_Val,500,1020,0,Motorspeed);
         
     MotorLspeed = 0;
     
      MotorR1val = 0;
      MotorR2val = 1;
      MotorL1val = 0;
      MotorL2val = 1;   
      Engine();
      Serial.print("Backward Left");
     }   
     
/*************************************************************Rotate Right***************************************************************/ 
   if ( LYaxis_Val > 490 && LYaxis_Val < 520 && LXaxis_Val > 535)//Move Y-stick neutral + X-stick right    
   {
         
     MotorRspeed = map(LXaxis_Val,510,1020,0,Motorspeed);
      
     MotorLspeed = MotorRspeed;
        
      MotorR1val = 0;
      MotorR2val = 1;
      MotorL1val = 1;
      MotorL2val = 0;   
      Engine();
      Serial.print("Rotate Right");
   } 
   
/*************************************************************Rotate Left***************************************************************/    
 
   if ( LYaxis_Val > 490 && LYaxis_Val < 520 && LXaxis_Val < 495)//Move Y-stick nutral + X-stick right  
   {
     
     MotorRspeed = map(LXaxis_Val,0,510,Motorspeed,0);
        
     MotorLspeed = MotorRspeed;
    
      MotorR1val = 1;
      MotorR2val = 0;
      MotorL1val = 0;
      MotorL2val = 1;  
      Engine();
      Serial.print("Rotate Left"); 
   }     
   
/////////////////////////////////////////////Serial Print/////////////////////////////////////////////////////////////////////////// 
     
    Serial.print (">>>"); 
    Serial.print(MotorRspeed);
    Serial.print ("---"); 
    Serial.print(MotorLspeed);
    Serial.print ("---"); 
    Serial.print(MotorR1val);
    Serial.print ("---"); 
    Serial.print(MotorR2val);
    Serial.print ("---"); 
    Serial.print(MotorL1val);
    Serial.print ("---"); 
    Serial.println(MotorL2val);
  
  }// if ( radio.available() )

  else //No signal condition for safety
  {

   Serial.println ("No signal>>>Stop"); 
   MotorRspeed = 0;
   MotorLspeed = 0;
   MotorR1val = 0;
   MotorR2val = 0;
   MotorL1val = 0;
   MotorL2val = 0;
   Engine();
   
  }
  
}//void loop(void)

/*****************Driving motor *********************************************************************/

void Engine() {
//Right Motor 
  analogWrite(enableR , MotorRspeed);
  digitalWrite (MotorR1,MotorR1val);
  digitalWrite (MotorR2,MotorR2val);  
  delay (100);
 //Left Motor 
  analogWrite(enableL , MotorLspeed);
  digitalWrite (MotorL1,MotorL1val);
  digitalWrite (MotorL2,MotorL2val);  
  delay (100);
}

