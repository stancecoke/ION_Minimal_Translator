//https://github.com/stancecoke/ION_Minimal_Translator
//by stancecoke

#include <Arduino.h>

#include "crc8.h"

HardwareSerial hwSerCntrl(1);
uint8_t receiveBuffer[255];
uint8_t transmitBuffer[255];
uint8_t BatteryToMotor[22][17]={
  {0x10,0x01,0x20,0x30,0x14}, //00
  {0x10,0x01,0x20,0x32,0x75}, //01
  {0x10,0x01,0x20,0x33,0xE4}, //02
  {0x10,0x01,0x21,0x31,0x00,0x22}, //03
  {0x10,0x01,0x21,0x34,0x01,0x7F}, //04
  {0x10,0x01,0x21,0x34,0x02,0x8F}, //05
  {0x10,0x01,0x21,0x34,0x03,0x1E}, //06
  {0x10,0x01,0x23,0x08,0x48,0x4D,0x00,0x10}, //07
  {0x10,0x01,0x23,0x08,0x48,0x4D,0x02,0x71}, //08
  {0x10,0x01,0x23,0x08,0x48,0x4D,0x04,0xD2}, //09
  {0x10,0x01,0x23,0x08,0x48,0x4E,0x00,0x54}, //10
  {0x10,0x01,0x23,0x08,0x48,0x4E,0x02,0x35}, //11
  {0x10,0x01,0x23,0x08,0x48,0x4E,0x04,0x96}, //12
  {0x10,0x01,0x28,0x09,0x94,0xB0,0x09,0xC4,0x14,0xB1,0x00,0xEC,0xDF}, //13
  {0x10,0x01,0x28,0x09,0x94,0xB0,0x09,0xC4,0x14,0xB1,0x01,0x00,0xAE}, //14
  {0x10,0x02,0x20,0x11,0x6F}, //15
  {0x10,0x02,0x20,0x12,0x9F}, //16
  {0x10,0x02,0x21,0x09,0x00,0xAB}, //17
  {0x10,0x02,0x2B,0x08,0x00,0x94,0x38,0x41,0x70,0x28,0x3A,0x3E,0x92,0x92,0xFA,0x09}, //18
  {0x10,0x02,0x2B,0x08,0x00,0x94,0x38,0x41,0x7C,0x28,0x3A,0x3E,0x92,0x92,0xFA,0x00}, //19
  {0x10,0x04,0x20,0xCC,0x00} //20
  };

  uint8_t nbBytes = 0;
  uint32_t lastTime = 0;
  uint32_t counter = 0;
  uint8_t length = 255;
  uint8_t len = 255;
  uint8_t crc = 0;
  uint8_t n = 0;


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
    //Serial.print(char(receiveBuffer[nbBytes]));
    //if(receiveBuffer[nbBytes]==0x0A)hwSerCntrl.printf("%d ",counter);
     //hwSerCntrl.println(nbBytes);
  
  if(nbBytes==2){
      length=receiveBuffer[nbBytes]&0xF; //Mask lower four bits for message lenght
      //hwSerCntrl.printf("erkannte Laenge!,%d\r\n",length);
      
    }  

  if(nbBytes==length+4){


     crc = crc8_bow((uint8_t *)&receiveBuffer,(length+4)); //Check CRC
      if(crc==receiveBuffer[nbBytes]){
        //Serial.println("Checksum OK!");
        //hwSerCntrl.println("Checksum OK!");
        //receiveBuffer[nbBytes+1]=0x0D;
        //receiveBuffer[nbBytes+2]=0x0A;

      } 
      else{
        //Serial.println("Checksum failed!");
        //hwSerCntrl.print("Checksum failed!\r");
      }
        for(int i=0;i<length+5;i++){
        //len = sprintf((uint8_t *)&transmitBuffer+3*i,"%02X",receiveBuffer[i])

        Serial.printf("%02X ",receiveBuffer[i]);
        }
        if(crc!=receiveBuffer[nbBytes]){
          Serial.printf("%02X ",0xFF);
        }
        Serial.printf("\r");
        //hwSerCntrl.write((uint8_t *)&receiveBuffer, length+7 );

    } 


 //hwSerCntrl.write((uint8_t *)&receiveBuffer, length+3 );
  
    if(receiveBuffer[nbBytes]==0x10&&nbBytes>3){
      nbBytes=0;
      receiveBuffer[nbBytes]=0x10;
      //Serial.println("Start detected!");
      //hwSerCntrl.println("Start detected!");
    }
    nbBytes++;
    
  }

    while (Serial.available())
  {
    // pipe the incoming byte to ION bus
        hwSerCntrl.print(char(Serial.read()));
  }

  if(millis()>lastTime+200){
    lastTime=millis();
    //Serial.println("Hello World!");
    //hwSerCntrl.println("Hello World!");
    hwSerCntrl.write((uint8_t *)&BatteryToMotor[n], (BatteryToMotor[n][2]&0x0F)+6);
    counter++;
    n++;
    if(n>21)n=0;
  }
}

