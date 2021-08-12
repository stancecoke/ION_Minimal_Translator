//https://github.com/stancecoke/ION_Minimal_Translator
//by stancecoke

#include <Arduino.h>
#include "crc8.h"

HardwareSerial hwSerCntrl(1);
uint8_t receiveBuffer[255];
  uint16_t nbBytes = 0;
  uint32_t lastTime = 0;
  uint8_t length = 255;
  uint8_t crc = 0;


void setup() {
  Serial.begin(115200);
  hwSerCntrl.begin(9600, SERIAL_8N1, 27, 14);
  Serial.println("Hello World!");
  hwSerCntrl.println("Hello World!");
}

void loop() {

  
  // Check for new data availability in the Serial buffer
  while (hwSerCntrl.available())
  {
    // Read the incoming byte
    receiveBuffer[nbBytes] = hwSerCntrl.read();  

  //BhwSerCntrl.println(nbBytes);
  
  if(nbBytes==2){
      length=receiveBuffer[nbBytes]&0xF; //Mask lower four bits for message lenght
      //hwSerCntrl.printf("erkannte Laenge!,%d\r\n",length);
      
    }  

  if(nbBytes==length+4){
     crc = crc8_bow((uint8_t *)&receiveBuffer,(length+4)); //Check CRC
      if(crc==receiveBuffer[nbBytes]){
        Serial.println("Checksum OK!");
        //hwSerCntrl.println("Checksum OK!");
        receiveBuffer[nbBytes+1]=0x0D;
        receiveBuffer[nbBytes+2]=0x0A;
        hwSerCntrl.write((uint8_t *)&receiveBuffer, length+7 );
      } 
      else{
        Serial.println("Checksum failed!");
        //hwSerCntrl.println("Checksum failed!");
      }
    } 


 //hwSerCntrl.write((uint8_t *)&receiveBuffer, length+3 );
  
    if(receiveBuffer[nbBytes]==0x10){
      nbBytes=0;
      receiveBuffer[nbBytes]=0x10;
      //Serial.println("Start detected!");
      //hwSerCntrl.println("Start detected!");
    }
    nbBytes++;
  }
  if(millis()>lastTime+1000){
    lastTime=millis();
    //Serial.println("Hello World!");
    //hwSerCntrl.printf("erkannte Laenge!%d, %d, %02X\r\n",nbBytes, length, crc);
  }
}