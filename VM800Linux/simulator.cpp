#include "simulator.h"

/**
******************************************************************************
* @details        Debug function to print car structure on a serial monitor  *
*                 console and to log file on SD car                          *
* @param          Car struct to print                                        *
******************************************************************************
*/
void printObj(struct car * obj){
  /*String command = "";
  
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
  
  command = "echo  Doors = ";
  command += obj->doors;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  command = "echo  Seatbelts = ";
  command += obj->seatbelts;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  command = "echo  Lights = ";
  command += obj->lights;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  command = "echo  R = ";
  command += obj->r;
  command += " >> /tmp/daniel.txt";
  system(command.buffer);
  command = "";
  
  system("echo +-------------------+ >> /tmp/daniel.txt");*/
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
    printf("Drzwi sie zmienily\n");
  
  if(tmp->seatbelts != audi->seatbelts)
    printf("Pasy sie zmienily\n"); 
  
  if(tmp->r != audi->r)
    printf("Wsteczny!!\n");
    
  if(tmp->lights != audi->lights)
    printf("Światła sie zmienily\n");
  
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
    printf("TempOut sie zmienilo\n");
  if(tmp->tempIn != audi->tempIn)
    printf("TempIn sie zmienilo\n");
  if(tmp->tempEngine != audi->tempEngine)
    printf("TempEngine sie zmienilo\n");
  
  save(audi, tmp);
  free(tmp);
  
  printObj(audi);
  digitalWrite(13, LOW);
}
