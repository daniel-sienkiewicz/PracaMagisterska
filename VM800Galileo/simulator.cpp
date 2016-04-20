/**
 * @file simulator.cpp
 * @author Daniel Sienkiewicz
 * @date 28 February 2016
 * @brief File containing declarations of all functions required to communication with car simulator.
 */

#include "simulator.h"

void printObj(struct car * obj, char * d){
  Serial.println("+--------------------+");                                  // DEBUG Print data into serial monitor console
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

  Serial.println(d);

  // Save data to file
  String file = "/tmp/";
  file += d;
  FILE *fp;
  char tmp[20];
  switch(dataFormat){
    case 1:                // CSV
            file += ".csv";
            file.toCharArray(tmp, 20);
            fp = fopen(tmp, "a");
            fseek(fp, 0L, SEEK_END);
            if(ftell(fp) == 0)
              fprintf(fp, "doors, seatbelts, lights, r, temp out, temp in, temp engine\n");

            fprintf(fp, "%d, %d, %d, %d, %f, %f, %f\n", obj->doors, obj->seatbelts, obj->lights, obj->r, obj->tempOut, obj->tempIn, obj->tempEngine);
            break;
    case 2:                // XML
            file += ".xml";
            file.toCharArray(tmp, 20);
            fp = fopen(tmp, "a");
            fprintf(fp, "<car>\n\t<doors> %d </doors>\n\t<seatbelts> %d </seatbelts>\n\t<lights> %d </lights>\n\t<r> %d </r>\n\t<Temp Out> %f </Temp Out>\n\t<Temp In> %f </Temp In>\n\t<Temp Engine> %f </Temp Engine>\n</car>\n", obj->doors, obj->seatbelts, obj->lights, obj->r, obj->tempOut, obj->tempIn, obj->tempEngine);
            break;
    case 3:                // JSON
            file += ".json";
            file.toCharArray(tmp, 20);
            fp = fopen(tmp, "a");
            fprintf(fp, "{ \"Car\" :{ \"doors \" : %d, \"seatbelts\" : %d, \"lights\" : %d, \"r\" : %d, \"Temp Out\" : %f, \"Temp In\" : %f, \"Temp Engine\" : %f}}\n", obj->doors, obj->seatbelts, obj->lights, obj->r, obj->tempOut, obj->tempIn, obj->tempEngine);
            break;
  }
  fclose(fp);
}

int readTemp(int portNumber){
  return analogRead(portNumber);
}

void save(struct car *audi, struct car *tmp){
  audi->doors = tmp->doors;
  audi->seatbelts = tmp->seatbelts;
  audi->lights = tmp->lights;
  audi->r = tmp->r;
  audi->tempOut = tmp->tempOut;
  audi->tempIn = tmp->tempIn;
  audi->tempEngine = tmp->tempEngine;
}

struct car * readData(){
  struct car * tmp = (struct car *)malloc(sizeof(struct car));

  int data = readPCF(0x71);                                                  // Read data from first PCF device
  tmp->doors = (data & 3) << 2;
  tmp->seatbelts = data & 12;

  data = readPCF(0x73);                                                      // Read data from second PCF device
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

void checkChangesDigital(){
  digitalWrite(13, HIGH);
  int change = 0;
  struct car * tmp = readData();
  Serial.println("Zamiana danych");
  Serial.println(tmp->doors);
  
  if(tmp->doors != audi->doors && screenNR == 1){
    Serial.println("Drzwi sie zmienily");
    change = 1;
  }
  if(tmp->seatbelts != audi->seatbelts && screenNR == 1){
    Serial.println("Pasy sie zmienily");
    change = 1;
  }
  if(tmp->lights != audi->lights && screenNR == 1){
    Serial.println("Światła sie zmienily");
    change = 1;
  }
  if(tmp->r != audi->r && screenNR == 1){
    Serial.println("Wsteczny!!");
    screenNR = 2;
    smartMirrorScreen();
    change = 2;
  }
  
  save(audi, tmp);
  free(tmp);
  sendData();

  digitalWrite(13, LOW);
  
  if(change == 1)
    mainScreen();
}

void sendData(){
  char buffer [1000];
  snprintf(buffer, sizeof(buffer), "curl -i -X POST -H 'Content-Type: application/json' -d '{\"tempIn\": \"%lf\", \"tempOut\" : \"%lf\", \"tempEngine\" : \"%lf\", \"GPSlongitude\" : \"%s\", \"GPSlatitude\" : \"%s\", \"doors\" : \"%d\", \"seatbelt\" : \"%d\", \"lights\" : \"%d\" }' http://localhost:3000/updateData", audi->tempIn, audi->tempOut, audi->tempEngine, "54.360N", "18.639E", audi->doors, audi->seatbelts, audi->lights);
  system(buffer);
}

void checkChangesAnalog(){
  digitalWrite(13, HIGH);
  struct car * tmp = readData();
  
  int change = 0;
  if((tmp->tempOut < audi->tempOut - 5 || tmp->tempOut > audi->tempOut + 5) && screenNR == 1){
    change = 1;
    Serial.println("TempOut sie zmienilo");
  }
    
  if((tmp->tempIn < audi->tempIn - 5 || tmp->tempIn > audi->tempIn + 5) && screenNR == 1){
    Serial.println("TempIn sie zmienilo");
    change =1 ;
  }
    
  if((tmp->tempEngine < audi->tempEngine - 5 || tmp->tempEngine > audi->tempEngine + 5) && screenNR == 1){
    Serial.println("TempEngine sie zmienilo");
    change = 1;
  }

  save(audi, tmp);
  free(tmp);
  
  if(change == 1)
    mainScreen();
  
  digitalWrite(13, LOW);
}
