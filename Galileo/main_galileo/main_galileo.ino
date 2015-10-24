/****************************************************************************
 * Copyright (C) 2015 by Daniel Sienkiewicz                                 *
 *                                                                          *
 *   Main_galileo is free software: you can redistribute it and/or modify it*
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   Main_galileo is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with Box. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

/**
 * @file main_galileo.ino
 * @author Daniel Sienkiewicz
 * @date 24 October 2015
 */

#include <TimerOne.h>

/**
******************************************************************************
* A global integer values                                                    *   
* Described which porets are used for communication with PCF                 *
******************************************************************************
*/
int sda = 8;
int scl = 9;
int pinInt0 = 2; 

/**
******************************************************************************
* Analog ports                                                               *
* A0 - temp Out                                                              *
* A1 - temp In                                                               *
* A2 - temp Engine                                                           *
******************************************************************************
*/

/**
******************************************************************************
* A global car structure                                                     *   
* Described main Car structure                                               *
******************************************************************************
*/
struct car {
 int doors;                                                                  // 5 doors
 int seatbelts;                                                              // 4 seatbelts
 int lights;                                                                 // Status of lights - turned on/offi           
 int r;                                                                      // Status of reverse gear 
 float tempOut;                                                              // The outside temperature 
 float tempIn;                                                               // The inside temperature
 float tempEngine;                                                           // The engine temperature
};
struct car *audi;                                                             // Main car structure

int d = 1;                                                                    // Delay time - for PCF handing

/**
******************************************************************************
* @details        Initialization function                                    *
******************************************************************************
*/
void setup() {
   pinMode(13, OUTPUT);    
   Serial.begin(9600);
   Serial.println("Setup...");
   pinMode(sda, OUTPUT);
   pinMode(scl, OUTPUT);
   digitalWrite(scl, HIGH);
   digitalWrite(sda, HIGH);
   
   audi = readData();
      
   attachInterrupt(pinInt0, checkChangesDigital, FALLING);                    // Set interrupt - changes in pin0 (HIGH) execute function checkChangesDigital()
   Timer1.initialize(500000);                                                 // Set a timer of length 100000 microseconds
   Timer1.attachInterrupt(checkChangesAnalog, 500000);                        // The callback will be called on each 5th timer interrupt, i.e. every 0.5 sec
}

/**
******************************************************************************
* @details        Reading value from analog ports (temperatures)             *
* @param          portNumber The number of the analog input pin to read      *
* @return         Value from the specified analog pin                        *
******************************************************************************
*/
int readTemp(int portNumber){
  return analogRead(portNumber);
}

/**
******************************************************************************
* @details        Reading value from PCF8574N I/O Expander                   *
* @param          adres The address of PCF8574N I/O Expander                 *
* @return         Value from the specified PCF8574N I/O Expander             *
******************************************************************************
*/
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

/**
******************************************************************************
* @details        Debug function to print car structure on a serial monitor  *
*                 console and to log file on SD car                          *
* @param          Car struct to print                                        *
******************************************************************************
*/
void printObj(struct car * obj){
  String command = "";
  
  Serial.println("+--------------------+");                                  // Print data into serial monitor console
  Serial.println("| Audi object:       |");
  Serial.print("| Doors = ");
  Serial.println(obj->doors);
  Serial.print("| Seatbelts = ");
  Serial.println(obj->seatbelts);
  Serial.print("| Lights = ");
  Serial.println(obj->lights);
  Serial.print("| R = ");
  Serial.println(obj->r);
  Serial.print("| Temp out = ");
  Serial.println(obj->tempOut);
  Serial.print("| Temp In = ");
  Serial.println(obj->tempIn);
  Serial.print("| Temp Engine = ");
  Serial.println(obj->tempEngine);
  Serial.println("+-------------------+");
  
  system("echo +--------------------+ >> /tmp/daniel.txt");                    // Print data into log file
  system("echo  Audi object:        >> /tmp/daniel.txt");
  
  command = "echo  Doors =";
  command += obj->doors;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  command = "echo  Seatbelts =";
  command += obj->seatbelts;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  command = "echo  Lights =";
  command += obj->lights;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  command = "echo  R =";
  command += obj->r;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  system("echo +-------------------+ >> /tmp/daniel.txt");
}

/**
******************************************************************************
* @details        Copying data function from temporary to main struct        *
* @param          *audi, *tmp Structures to and from which data are copied   *
******************************************************************************
*/
void save(struct car *audi, struct car *tmp){
  audi->doors = tmp->doors;
  audi->seatbelts = tmp->seatbelts;
  audi->lights = tmp->lights;
  audi->r = tmp->r;
  audi->tempOut = tmp->tempOut;
  audi->tempIn = tmp->tempIn;
  audi->tempEngine = tmp->tempEngine;
}

/**
******************************************************************************
* @details        Reading data about car status                              *
******************************************************************************
*/
struct car * readData(){
  struct car * tmp = (struct car *)malloc(sizeof(struct car));
  
  int data = readPCF(0x41);                                                  // Read data from first PCF device
  tmp->doors = (data & 3) << 2;
  tmp->seatbelts = data & 12;
  
  data = readPCF(0x43);                                                      // Read data from second PCF device
  tmp->doors |= data & 3;
  tmp->doors |= (data & 32) >> 1;
  tmp->seatbelts |= (data & 12) >> 2;
  tmp->lights = (data & 16) >> 4;
  tmp->r = (data & 64) >> 6;
  tmp->tempOut = readTemp(0);
  tmp->tempIn = readTemp(1);
  tmp->tempEngine = readTemp(2);
  return tmp;
}

/**
******************************************************************************
* @details        Check if sth on digital ports was changed                  *
******************************************************************************
*/
void checkChangesDigital(){
  digitalWrite(13, HIGH);
  struct car * tmp = readData();
  
  if(tmp->doors != audi->doors)
    Serial.println("Drzwi sie zmienily");
  if(tmp->seatbelts != audi->seatbelts)
    Serial.println("Pasy sie zmienily");
  if(tmp->r != audi->r)
    Serial.println("Wsteczny!!");
  if(tmp->lights != audi->lights)
    Serial.println("Światła sie zmienily");
  
  save(audi, tmp);
  free(tmp);

  printObj(audi);
  digitalWrite(13, LOW);
}

/**
******************************************************************************
* @details        Check if sth on analog ports was changed                  *
******************************************************************************
*/
void checkChangesAnalog(){
  digitalWrite(13, HIGH);
  struct car * tmp = readData();
  
  if(tmp->tempOut != audi->tempOut)
    Serial.println("TempOut sie zmienilo");
  if(tmp->tempIn != audi->tempIn)
    Serial.println("TempIn sie zmienilo");
  if(tmp->tempEngine != audi->tempEngine)
    Serial.println("TempEngine sie zmienilo");
  
  save(audi, tmp);
  free(tmp);
  
  printObj(audi);
  digitalWrite(13, LOW);
}

/**
******************************************************************************
* @details        Main function                                              *
******************************************************************************
*/
void loop() {
  
  /* Version with ask about changes in loop
  * checkChangesDigital();
  * checkChangesAnalog();
  * delay(1000);
  */
  
}
