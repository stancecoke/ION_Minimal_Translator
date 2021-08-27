//https://github.com/stancecoke/ION_Minimal_Translator
//by stancecoke

#include <Arduino.h>

#include "crc8.h"

void printLatestMessage(void);

HardwareSerial hwSerCntrl(1);
uint8_t receiveBuffer[255];
uint8_t lastMessage[255];
uint8_t lastMessageLength;
uint8_t transmitBuffer[255];
uint8_t startSequence[255]={0,14,25,20,23,22,21};
uint8_t runSequence[255]={13,21,21,21,21,21,21,21};
uint8_t systemState=0;
uint8_t BatteryToMotor[26][17]={
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
  {0x10,0x02,0x20,0x12,0x9F}, //16  10 02 20 12 9F 
  {0x10,0x02,0x21,0x09,0x00,0xAB}, //17, 10 02 21 09 00 AB
  {0x10,0x02,0x2B,0x08,0x00,0x94,0x38,0x41,0x4C,0x28,0x3A,0x3E,0x92,0x3B,0x99,0x41}, //18, 10 02 2B 08 00 94 38 41 4C 28 3A 3E 92 3B 99 41 
  {0x10,0x02,0x2B,0x08,0x00,0x94,0x38,0x41,0x7C,0x28,0x3A,0x3E,0x92,0x92,0xFA,0x00}, //19
  {0x10,0x04,0x20,0xCC}, //20
  {0x10,0x00,0xB1}, //21
  {0x10,0x60,0x99}, //22
  {0x10,0x40,0x40}, //23
  {0x10,0x44,0x20,0x19}, //24
  {0x10,0x64,0x20,0xD2} //25
  };

  uint8_t nbBytes = 0;
  uint8_t receivedByte = 0;
  uint32_t lastTime = 0;
  uint32_t counter = 0;
  uint8_t length = 255;
  uint8_t len = 255;
  uint8_t crc = 0;
  uint8_t n = 0;
  uint8_t messageCounter = 0;
  uint8_t startNewMessage = 0;
  uint8_t NewMessageFlag = 0;
  uint8_t UARTidleFlag = 0;


void setup() {
  Serial.begin(115200);
  hwSerCntrl.begin(9600, SERIAL_8N1, 27, 14);
  Serial.println("Hello World!");
  //hwSerCntrl.println("Hello World!");


}

void loop() {

  
  // Check for new data availability in the Serial buffer
  while (hwSerCntrl.available())
  {

    if(UARTidleFlag){
     // if((millis()-counter)>3)Serial.printf("\r Idle millis: %ld ",millis()-counter);
      UARTidleFlag=0;
    }
    // Read the incoming byte
    receiveBuffer[nbBytes] = hwSerCntrl.read();  
    
  if(nbBytes==2){
      if((receiveBuffer[nbBytes]>>4)>2&&(receiveBuffer[nbBytes]>>4)<0x0C){ //Nachrichten mit 3Bytes auffangen
        lastMessageLength=3;
        printLatestMessage();
      }

      else length=receiveBuffer[nbBytes]&0xF; //Mask lower four bits for message length
      //hwSerCntrl.printf("erkannte Laenge!,%d\r\n",length);
  }  

  if(nbBytes==3&&(receiveBuffer[1]&0x0F)>2){ //Nachrichten mit 4Bytes auffangen
        lastMessageLength=4;
        printLatestMessage();
  }

  if(nbBytes==length+4){


     crc = crc8_bow((uint8_t *)&receiveBuffer,(length+4)); //Check CRC
      if(crc==receiveBuffer[nbBytes]){
        lastMessageLength=nbBytes+1;
        printLatestMessage();
      } 
      else{
        //Serial.println("Checksum failed!");
        //hwSerCntrl.print("Checksum failed!\r");
      }

    } 

    
    
  
    if(receiveBuffer[nbBytes]==0x10){
      
      Serial.printf("\r");
      nbBytes=0;
      receiveBuffer[nbBytes]=0x10;
      //startNewMessage=1;
      //Serial.println("Start detected!");
      //hwSerCntrl.println("Start detected!");
    }
    Serial.printf("%02X ",receiveBuffer[nbBytes]);
    nbBytes++;
    
  }

    while (Serial.available())
  {
    
      receivedByte=Serial.read();
      //pipe the incoming byte to ION bus
      if(systemState==2)hwSerCntrl.print(char(receivedByte));
      
  }

  if(millis()>lastTime+200){
    lastTime=millis();
    //Serial.println("Hello World!");
    //hwSerCntrl.println("Hello World!");
    if (receivedByte==0xFA){
      if(!systemState){
          delayMicroseconds(50000);
          if(BatteryToMotor[startSequence[n]][2]==0xB1)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 3);
          else if((BatteryToMotor[startSequence[n]][1]&0x0F)==0x04)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 4);
          else if(BatteryToMotor[startSequence[n]][2]==0x40)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 3);
          else if(BatteryToMotor[startSequence[n]][2]==0x99)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 3);
          else hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], (BatteryToMotor[startSequence[n]][2]&0x0F)+5);
          
          n++;
        if(n>6){
          n=0;  
          systemState=2;
        }
      } 
      if(systemState==1){
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[runSequence[n]], (BatteryToMotor[runSequence[n]][2]&0x0F)+6);
        n++;
        if(n>8)n=0;

      }
    } 
    if (receivedByte==0xFB){
      systemState=0;
      receivedByte=0;
      n=0;
    }
    if (receivedByte==0xFC){
      systemState=2;
    }
    counter++;

  }
  if(!hwSerCntrl.available()&&!UARTidleFlag){
    UARTidleFlag=1;
    counter= millis();
  }

  //if((millis()-counter)>50&&systemState==2)hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3); //if system pauses restart

  if(systemState==2&&!hwSerCntrl.available()&&NewMessageFlag){
    NewMessageFlag=0;
    //Antwort auf 10 40 40 (Fehlermeldung?!) --> Stopp-Signal zurücksenden
    if(lastMessage[1]==0x40&&lastMessage[2]==0x40){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[22], 3);     
    }
    //Antwort auf 10 20 68
    else if(lastMessage[1]==0x20&&lastMessage[2]==0x68){
      
      
      messageCounter++;
      if (messageCounter>100){
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[24], 4);
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);
        messageCounter=0;
      }

      else if (messageCounter==50){
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[6], 6);
      }

      else  hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);
      
      
    }

    //Antwort auf 10 21 04 08 94 38 28 3A D7
    else if(lastMessage[1]==0x21&&lastMessage[2]==0x04){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[18], (BatteryToMotor[18][2]&0x0F)+5);
      
    }  
    

    //Antwort auf 10 21 0A 09: 10 02 21 09 00 AB 
    else if(lastMessage[1]==0x21&&lastMessage[2]==0x0A){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[17], (BatteryToMotor[17][2]&0x0F)+5);
        
    }

    //Antwort auf 10 21 01 12 00 D0 
    else if(lastMessage[1]==0x21&&lastMessage[2]==0x01){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[16], (BatteryToMotor[16][2]&0x0F)+5);
           
    }

        //Neustart nach Antwort 10 22 00 34 49  
    else if(lastMessage[1]==0x22&&lastMessage[2]==0x00){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);           
    }
    
    }
}

void printLatestMessage(void){
      
      memcpy(lastMessage, receiveBuffer, lastMessageLength);
      /*
      for(int i =0; i<lastMessageLength; i++){
        Serial.printf("%02X ",lastMessage[i]);
      }
      Serial.printf("\r");*/
      NewMessageFlag=1;

}

