#include "I2C.h"

/**
******************************************************************************
* @details        Reading value from PCF8574N I/O Expander                   *
* @param          adres The address of PCF8574N I/O Expander                 *
* @return         Value from the specified PCF8574N I/O Expander             *
******************************************************************************
*/
int d = 1;                                                                    // Delay time - for PCF handing

int readPCF(char adres){
  int m, ack, wynik = 0;
  digitalWrite(sda, LOW);
  delay(d);
  digitalWrite(scl, LOW);

  for(m = 0x80; m; m >>= 1){                                                 // Address transfer MSB->LSB
    if(adres & m)         
      digitalWrite(sda,HIGH);
    else
      digitalWrite(sda,LOW);
        
   digitalWrite(scl,HIGH);                                                   // Generate clock pulse
   delay(d);
   digitalWrite(scl,LOW); 
   delay(d);
  }

   pinMode(sda,INPUT);
   digitalWrite(scl,HIGH);
   delay(d);
   
   ack = digitalRead(sda);                                                    //Read ACL
   digitalWrite(scl,LOW);
   delay(d);
   
   for(m = 0x80; m; m>>=1){
     digitalWrite(scl,HIGH);
     delay(d);
    
     if(digitalRead(sda)){
       wynik |= m;
     }
     
     digitalWrite(scl, LOW);
     delay(d);
   }
   
   pinMode(sda,OUTPUT);                                                       // Generate STOP bit
   digitalWrite(scl, HIGH);
   delay(d);
   digitalWrite(sda, HIGH);
   delay(d);
   return wynik;
}
