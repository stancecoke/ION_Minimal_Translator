# ION_Minimal_Translator
I stopped working on this project, but other guys made it work meanwhile:  
https://github.com/void-spark/SpartaIonStuff
Running an ION EBike-Motor with a NoName batterie and a Kunteng display  
Based on an ESP32 Wroom  
Status: Not working yet :-(  
sending sniffed data to ESP USB interface @115200 BAUD  

control by sending single Bytes to ESP:  
Hex-command via ESP32 USB   
0xFA Start  
0xFB Reset  
0xFC transfer USB-UART Bytes to motor  
0xFD reactivate automatc answer  
0xFE Quit: 10 01 21 31 00 22  
0xFF send assistance level: 10 01 21 34 01 7F (is this really the assistance level?!)  

inspired by 
https://github.com/InfantEudora/sparta_ion
