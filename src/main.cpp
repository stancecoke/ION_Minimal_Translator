//https://github.com/stancecoke/ION_Minimal_Translator
//by stancecoke

#include <Arduino.h>
#include <EEPROM.h>
#include "crc8.h"

void printLatestMessage(void);

HardwareSerial hwSerCntrl(1);
uint8_t receiveBuffer[255];
uint8_t lastMessage[255];
uint8_t lastMessageLength;
uint8_t transmitBuffer[255];
uint8_t startSequence[255]={27,28,24,24,0,14,25,25,20,21};
uint8_t runSequence[255]={13,21,21,21,21,21,21,21};
uint8_t systemState=0;
uint8_t DisplayAndBattery[35][17]={
  {0x10, 0xC1, 0x21, 0x22, 0x80, 0x5F, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0xD4, 0xC4, },
{0x10, 0xC1, 0x29, 0x26, 0x03, 0x30, 0xC0, 0x00, 0xC0, 0x00, 0xFC, 0xCC, 0xC1, 0xE1, },
{0x10, 0x22, 0xC0, 0x26, 0xD9, },
{0x10, 0xC1, 0x22, 0x25, 0x04, 0x08, 0x84, },
{0x10, 0x22, 0xC0, 0x25, 0x29, },
{0x10, 0xC1, 0x21, 0x22, 0x01, 0x6F, },
{0x10, 0x22, 0xC2, 0x22, 0x02, 0xD4, 0x7D, },
{0x10, 0xC1, 0x29, 0x26, 0x0C, 0x30, 0xC3, 0x5C, 0xC0, 0x00, 0xFC, 0xCC, 0xC1, 0xB7, },
{0x10, 0x22, 0xC0, 0x26, 0xD9, },
{0x10, 0xC1, 0x29, 0x27, 0x03, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x3C, 0xCC, 0xC1, 0x45, },
{0x10, 0x22, 0xC0, 0x27, 0x48, },
{0x10, 0x44, 0x20, 0x19, },
{0x10, 0x44, 0x20, 0x19, },
{0x10, 0xC1, 0x21, 0x22, 0x02, 0x9F, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0xD9, 0x50, },
{0x10, 0x01, 0x20, 0x30, 0x14, },
{0x10, 0x01, 0x28, 0x09, 0x94, 0xB0, 0x09, 0xC4, 0x14, 0xB1, 0x01, 0x09, 0xF8, },
{0x10, 0xC1, 0x21, 0x22, 0x03, 0x0E, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0xFF, 0x2A, },
{0x10, 0xC1, 0x29, 0x26, 0x0C, 0x0C, 0xC3, 0x5C, 0xC0, 0x00, 0xF0, 0x76, 0x59, 0xA0, },
{0x10, 0x22, 0xC0, 0x26, 0xD9, },
{0x10, 0xC1, 0x21, 0x22, 0x04, 0x3C, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0x01, 0x0A, },
{0x10, 0x64, 0x20, 0xD2, },
{0x10, 0x64, 0x20, 0xD2, },
{0x10, 0xC1, 0x21, 0x22, 0x05, 0xAD, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0x09, 0xCD, },
{0x10, 0xC1, 0x21, 0x22, 0x06, 0x5D, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0x0B, 0xAC, },
{0x10, 0xC1, 0x21, 0x22, 0x07, 0xCC, },
{0x10, 0xC1, 0x21, 0x22, 0x07, 0xCC, },
{0x10, 0x22, 0xC2, 0x22, 0x00, 0x12, 0x37, },
{0x10, 0x04, 0x20, 0xCC, },
{0x10,0x00,0xB1}
};
uint8_t BatteryToMotor[29][17]={
  {0x10,0x01,0x20,0x30,0x14}, //00
  {0x10,0x01,0x20,0x32,0x75}, //01
  {0x10,0x01,0x20,0x33,0xE4}, //02
  {0x10,0x01,0x21,0x31,0x00,0x22}, //03 Aufforderung zum Senden der Abschaltmeldung 10 21 0A 09 94 38 ....
  {0x10,0x01,0x21,0x34,0x01,0x7F}, //04
  {0x10,0x01,0x21,0x34,0x02,0x8F}, //05
  {0x10,0x01,0x21,0x34,0x03,0x1E}, //06
  {0x10,0x01,0x23,0x08,0x48,0x4D,0x00,0x10}, //07
  {0x10,0x01,0x23,0x08,0x48,0x4D,0x02,0x71}, //08
  {0x10,0x01,0x23,0x08,0x48,0x4D,0x04,0xD2}, //09
  {0x10,0x01,0x23,0x08,0x48,0x4E,0x00,0x54}, //10
  {0x10,0x01,0x23,0x08,0x48,0x4E,0x02,0x35}, //11
  {0x10,0x22,0xC0,0x26,0xD}, //12 answer to display update
  {0x10,0xC1,0x29,0x26,0xC0,0x30,0xC3,0x43,0xC1,0x99,0xFC,0xCC,0xC0,0x8}, //13 display update 10 C1 29 26 C0 30 C3 43 C1 99 FC CC C0 86 
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
  {0x10,0x64,0x20,0xD2}, //25
  {0x10, 0xC1, 0x29, 0x26, 0xC0, 0x30, 0xC3, 0x3F, 0xC0, 0x00, 0xFC, 0xCC, 0xC0, 0x73}, //26
  {0x10,0xC1,0x21,0x22,0x80,0x5F}, //27
  {0x10, 0x22, 0xC2, 0x22, 0x00, 0xEE, 0x76} //28
  };

  uint8_t nbBytes = 0;
  uint8_t receivedByte = 0;
  uint32_t lastTime = 0;
  uint16_t speed = 0;
  uint32_t counter = 0;
  uint8_t length = 255;
  uint8_t len = 255;
  uint8_t crc = 0;
  uint8_t n = 0;
  uint8_t messageCounter = 0;
  uint8_t displayCounter = 0;
  uint8_t startNewMessage = 0;
  uint8_t NewMessageFlag = 0;
  uint8_t UARTidleFlag = 0;
  enum EEPROMAddresses { B0V, B1V, B0H, B1H, B2H, B3H };


   

void setup() {
  Serial.begin(115200);
  hwSerCntrl.begin(9600, SERIAL_8N1, 27, 14);
  Serial.println("Hello World!");
  //hwSerCntrl.println("Hello World!");
  //Krücke zum händischen Erzeugen der letzten Nachricht beim Abschalten
  /*  EEPROM.begin(6);

      EEPROM.write(B0V,0x41);
      EEPROM.write(B1V,0x35);
      EEPROM.write(B0H,0x3E);
      EEPROM.write(B1H,0x90);
      EEPROM.write(B2H,0x00);
      EEPROM.write(B3H,0xD8);
      Serial.printf("%02X ",EEPROM.read(B3H));
      EEPROM.end();
  
  */

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
      if(systemState==3)hwSerCntrl.print(char(receivedByte));
      
  }

  if(millis()>lastTime+200){
    lastTime=millis();
    //Serial.println("Hello World!");
    //hwSerCntrl.println("Hello World!");
    if (receivedByte==0xFA){
      if(!systemState){
          delayMicroseconds(50000);
          
          if(DisplayAndBattery[n][2]==0xB1)hwSerCntrl.write((uint8_t *)&DisplayAndBattery[n], 3);
          else if((DisplayAndBattery[n][1]&0x0F)==0x04)hwSerCntrl.write((uint8_t *)&DisplayAndBattery[n], 4);
          else if(DisplayAndBattery[n][2]==0x40)hwSerCntrl.write((uint8_t *)&DisplayAndBattery[n], 3);
          else if(DisplayAndBattery[n][2]==0x99)hwSerCntrl.write((uint8_t *)&DisplayAndBattery[n], 3);
          else hwSerCntrl.write((uint8_t *)&DisplayAndBattery[n], (DisplayAndBattery[n][2]&0x0F)+5);
          /*if(BatteryToMotor[startSequence[n]][2]==0xB1)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 3);
          else if((BatteryToMotor[startSequence[n]][1]&0x0F)==0x04)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 4);
          else if(BatteryToMotor[startSequence[n]][2]==0x40)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 3);
          else if(BatteryToMotor[startSequence[n]][2]==0x99)hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], 3);
          else hwSerCntrl.write((uint8_t *)&BatteryToMotor[startSequence[n]], (BatteryToMotor[startSequence[n]][2]&0x0F)+5);
          */
          
          n++;
        if(n>34){
          n=0;  
          systemState=2;
        }
      } 

    } 
    if (receivedByte==0xFB){
      systemState=0;
      receivedByte=0;
      n=0;
    }
    if (receivedByte==0xFC){
      systemState=3;
    }
    if (receivedByte==0xFD){
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
    //Antwort auf 10 40 40 (Fehlermeldung?!) --> Stopp-Signal zurücksenden und neu starten
    if(lastMessage[1]==0x40&&lastMessage[2]==0x40){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[22], 3); 
      delay(2);
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);    
    }
    //Antwort auf 10 20 68
    else if(lastMessage[1]==0x20&&lastMessage[2]==0x68){
      
      
      messageCounter++;
      if (messageCounter>100){ // send battery to display upcounting command and answer from display
        memcpy(transmitBuffer, BatteryToMotor[27], ((BatteryToMotor[27][2]&0x0F)+5));
        displayCounter++;
        transmitBuffer[4]= displayCounter;
        transmitBuffer[5] = crc8_bow((uint8_t *)&transmitBuffer,(transmitBuffer[2]&0x0F)+4);
        hwSerCntrl.write((uint8_t *)&transmitBuffer, (transmitBuffer[2]&0x0F)+5);
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[28], (BatteryToMotor[28][2]&0x0F)+5); 
        delay(5);
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);
        messageCounter=0;
        if(displayCounter>0x0F)displayCounter=0;
      }

      else if (messageCounter==50){//send display update command and answer form display (speed in km/h+10 as decimal digits in bytes 8 + 9)
        memcpy(transmitBuffer, BatteryToMotor[13], ((BatteryToMotor[13][2]&0x0F)+5));
        transmitBuffer[8]=(speed/100)&0x0F; //100er Stelle
        transmitBuffer[9]=((speed-100*transmitBuffer[8])<<8)|(speed % 10);//10er stelle in oberen 4 bits, 1er Stelle in unteren 4 bits
        transmitBuffer[13] = crc8_bow((uint8_t *)&transmitBuffer,(transmitBuffer[2]&0x0F)+4);
        hwSerCntrl.write((uint8_t *)&transmitBuffer, (transmitBuffer[2]&0x0F)+5);
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[12], (BatteryToMotor[12][2]&0x0F)+5); //answer from display
        delay(5);
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);
        
      }


      else if (messageCounter==75||messageCounter==78){
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[25], 4);
        delay(2);
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);
      }
      else if (receivedByte==0xFE){ //Abschaltsignal senden
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[3], (BatteryToMotor[3][2]&0x0F)+5); 
        
        
      }
      else if (receivedByte==0xF9){ //Abschaltsignal senden
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[22], 3);         
        receivedByte=0xFD;
      }
     
      else if (receivedByte==0xFF){ //Einschaltsignal senden
        hwSerCntrl.write((uint8_t *)&BatteryToMotor[4], (BatteryToMotor[4][2]&0x0F)+5); 
        receivedByte=0xFD;
      }

      else  hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);
      
      
    }


    //Antwort auf 10 21 04 08 94 38 28 3A D7
    else if(lastMessage[1]==0x21&&lastMessage[2]==0x04){
      memcpy(transmitBuffer, BatteryToMotor[18], ((BatteryToMotor[18][2]&0x0F)+5));
      EEPROM.begin(6);

      // Werte vom letzen Einschalten wieder Einlesen
     
      transmitBuffer[7]=(EEPROM.read(B0V));
      transmitBuffer[8]=(EEPROM.read(B1V));
      transmitBuffer[11]=(EEPROM.read(B0H));
      transmitBuffer[12]=(EEPROM.read(B1H));
      transmitBuffer[13]=(EEPROM.read(B2H));
      transmitBuffer[14]=(EEPROM.read(B3H));
      //Serial.printf("%02X ",transmitBuffer[14]);


      EEPROM.end();
      transmitBuffer[15] = crc8_bow((uint8_t *)&transmitBuffer,(transmitBuffer[2]&0x0F)+4);
      hwSerCntrl.write((uint8_t *)&transmitBuffer, (transmitBuffer[2]&0x0F)+5);
      
    }  

        //Antwort auf 10 21 0A 09 94 38 xx xx 28 3A xx xx xx xx 94 
    else if(lastMessage[1]==0x21&&lastMessage[5]==0x38){
      
      EEPROM.begin(6);

      // Werte beim Abschalten wegschreiben
      EEPROM.write(B0V,lastMessage[6]);
      EEPROM.write(B1V,lastMessage[7]);
      EEPROM.write(B0H,lastMessage[10]);
      EEPROM.write(B1H,lastMessage[11]);
      EEPROM.write(B2H,lastMessage[12]);
      EEPROM.write(B3H,lastMessage[13]);

      EEPROM.end();
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[17], (BatteryToMotor[17][2]&0x0F)+5);
      receivedByte=0xF9;
    }
    

    //Antwort auf 10 21 0A 09: 10 02 21 09 00 AB 
    else if(lastMessage[1]==0x21&&lastMessage[5]==0xC0){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[17], (BatteryToMotor[17][2]&0x0F)+5);
      speed= (lastMessage[6]<<8)+lastMessage[7];  
    }

    //Antwort auf 10 21 01 12 00 D0 
    else if(lastMessage[1]==0x21&&lastMessage[2]==0x01){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[16], (BatteryToMotor[16][2]&0x0F)+5);
      //delay(2);
      //hwSerCntrl.write((uint8_t *)&BatteryToMotor[4], (BatteryToMotor[4][2]&0x0F)+5);     
    }

    //Neustart nach Antwort 10 22 00 34 49  
    else if(lastMessage[1]==0x22&&lastMessage[2]==0x00){
      hwSerCntrl.write((uint8_t *)&BatteryToMotor[21], 3);           
    }

    //Neustart nach Antwort 10 23 00 AB
    else if(lastMessage[1]==0x23&&lastMessage[2]==0x00){
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

