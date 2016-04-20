/**
 * @file FT800.cpp
 * @author Daniel Sienkiewicz
 * @date 28 February 2016
 * @brief File containing declarations of all functions required to use with VM800.
 */

#include "FT800.h"
#import <Arduino.h>

void delay_us(int us){
  delayMicroseconds(us);
}

void delay_ms(int ms){
  delay(ms);
}

void sendData(int data){
  int i;
  for(i = 0x80; i; i >>= 1){
    digitalWrite(xSDO, data & i);
    delay_us(10);
    digitalWrite(xclock, HIGH);
    delay_us(10);
    digitalWrite(xclock, LOW);
    delay_us(10);
  }
}

unsigned char getData(){
  int i, j;
  unsigned char result = 0;
  for(i = 0x80; i; i >>= 1){
    if(digitalRead(xSDI)){
      result |= i;
    }
    digitalWrite(xclock, HIGH);
    delay_ms(2);
    digitalWrite(xclock, LOW);
  }
  return result;
}

void ft800memWrite8(unsigned long ftAddress, unsigned char ftData8){
#ifdef PIC
  RC6=0;                    			   			 // Set CS# low

  delay_us(2);
  SSPBUF=((char)(ftAddress>>16)|MEM_WRITE);         // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress>>8));		    		// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);			    			// Send low address byte
  while(!SSPSTATbits.BF);
  SSPBUF=ftData8;				    				// Send data byte
  while(!SSPSTATbits.BF);
  delay_us(2);

  RC6=1;                                            // Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_WRITE));
  sendData((ftAddress >> 8));
  sendData(ftAddress);
  sendData(ftData8);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

void ft800memWrite16(unsigned long ftAddress, unsigned int ftData16){
#ifdef PIC
  RC6=0;					      				// Set CS# low

  SSPBUF=((char)(ftAddress>>16)|MEM_WRITE);     // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress>>8));		      	// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress));			      	// Send low address byte
  while(!SSPSTATbits.BF);

  SSPBUF=((char)(ftData16));		     	    // Send data low byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData16>>8));			        // Send data high byte
  while(!SSPSTATbits.BF);
  delay_us(2);

  RC6=1;                                        // Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_WRITE));
  sendData((ftAddress >> 8));
  sendData((ftAddress));

  sendData(ftData16);
  sendData((ftData16 >> 8));
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

void ft800memWrite32(unsigned long ftAddress, unsigned long ftData32){
#ifdef PIC
  RC6=0;   					        					// Set CS# low

  SSPBUF=((char)(ftAddress>>16)|MEM_WRITE);             // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress>>8));						// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress));							// Send low address byte
  while(!SSPSTATbits.BF);

  SSPBUF=((char)(ftData32));							// Send data low byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData32>>8)); 						// Send data middle-low byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData32>>16));						// Send data middle-high byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData32>>24));						// Send data high byte
  while(!SSPSTATbits.BF);
  delay_us(2);
  RC6=1;												// Set CS# high

#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_WRITE));
  sendData((ftAddress >> 8));
  sendData(ftAddress);

  sendData(ftData32);
  sendData((ftData32 >> 8));
  sendData((ftData32 >> 16));
  sendData((ftData32 >> 24));
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

unsigned char ft800memRead8(unsigned long ftAddress){
  unsigned char ftData8=ZERO;
#ifdef PIC
  RC6=0;											// Set CS# low

  SSPBUF=(char)(ftAddress>>16)|MEM_READ;	        // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress>>8);						// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);							// Send low address byte
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;					        			// Send dummy byte
  while((SSPSTAT&(1<<0))==0);                       //to samo co while(!SSPSTATbits.BF) ??
  ftData8=SSPBUF;
  RC6=1;											// Set CS# high

#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_READ)&255);
  sendData((ftAddress >> 8)&255);
  sendData(ftAddress&255);
  sendData(0);
  delay_us(2);

  ftData8 = getData();
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
  return(ftData8);			                	// Return byte read
}

unsigned char ft800memRead16(unsigned long ftAddress){
  unsigned int ftData16, tempData[2];
  ftData16=0x0000;
#ifdef PIC
  RC6=0;											// Set CS# low

  SSPBUF=(char)(ftAddress>>16)|MEM_READ;	        // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress>>8);						// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);							// Send low address byte
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;  									// Send dummy byte
  SSPBUF=ZERO;					       				// Send dummy byte
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;					        			// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[0]=SSPBUF;
  ftData16=(tempData[0])|(ftData16);

  SSPBUF=ZERO;										// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[1]=SSPBUF;
  ftData16=(ftData16)|(tempData[1]<<8);

  delay_us(2);										// Read data byte
  RC6=1;											// Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_READ)&255);
  sendData((ftAddress >> 8)&255);
  sendData(ftAddress&255);
  sendData(0);
  delay_us(2);

  tempData[0] = getData();
  ftData16 = (tempData[0])|(ftData16);
  sendData(0);
  delay_us(2);

  tempData[1] = getData();
  ftData16 = (ftData16) | (tempData[1] << 8);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
  return(ftData16);								// Return 16 bits
}

unsigned long ft800memRead32(unsigned long ftAddress){
  unsigned long ftData32, tempData[4];

  ftData32=0x00000000;

#ifdef PIC
  RC6=0;											// Set CS# low

  SSPBUF=(char)(ftAddress>>16)|MEM_READ;	        // Send Memory Read plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress>>8);						// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);							// Send low address byte
  while(!SSPSTATbits.BF);
 SSPBUF=ZERO;                         		        //send dummy byte
  delay_us(2);

  SSPBUF=ZERO;  									// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[3]=SSPBUF;								// Read data byte
  ftData32=tempData[3]|ftData32;
  SSPBUF=ZERO;  									// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[2]=SSPBUF;								// Read data byte
  ftData32=(tempData[2]<<8)|ftData32;
  SSPBUF=ZERO;  									// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[1]=SSPBUF;                               // Read data byte
  ftData32=(tempData[1]<<16)|ftData32;
  SSPBUF=ZERO;  									// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[0]=SSPBUF;                               // Read data byte
  ftData32=ftData32|(tempData[0]<<24);

  delay_us(2);
  RC6=1;											// Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_READ));
  sendData((ftAddress >> 8));
  sendData(ftAddress);
  sendData(0);
  delay_us(2);

  tempData[3] = getData();
  ftData32 = tempData[3] | ftData32;
  sendData(0);
  delay_us(2);

  tempData[2] = getData();
  ftData32 = (tempData[2] << 8) | ftData32;
  sendData(0);
  delay_us(2);

  tempData[1] = getData();
  ftData32 = (tempData[1] << 16) | ftData32;
  sendData(0);
  delay_us(2);

  tempData[0] = getData();
  ftData32 = ftData32 | (tempData[0] << 24);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
  return(ftData32);
}

unsigned int incCMDOffset(unsigned int currentOffset, unsigned char commandSize){
  unsigned int newOffset;								// used to hold new offset

  newOffset=currentOffset+commandSize;	                // Calculate new offset
  if(newOffset>4095){									// If new offset past boundary...
    newOffset=(newOffset-4096);				            // ... roll over pointer
  }
  return(newOffset);									// Return new offset
}

void ft800cmdWrite(unsigned char ftCommand){
#ifdef PIC
  RC6=0;												// Set CS# low

  SSPBUF=ftCommand;					        			// Send command
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;
  while(!SSPSTATbits.BF);		                        // Commands consist of two more zero bytes
  SSPBUF=ZERO;
  while(!SSPSTATbits.BF);		                        // Send last zero byte

  RC6=1;												// Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(ftCommand);
  delay_us(2);
  sendData(0);
  delay_us(2);
  sendData(0);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}
