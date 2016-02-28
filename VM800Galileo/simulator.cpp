#include "simulator.h"

void printObj(struct car * obj){
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
  
  // Save data to file
  FILE *fp;
  switch(dataFormat){
    case 1:                // CSV
            
            fp = fopen("/tmp/carData.csv", "a");
            fseek(fp, 0L, SEEK_END);
            if(ftell(fp) == 0)
              fprintf(fp, "doors, seatbelts, lights, r\n");
            
            fprintf(fp, "%d, %d, %d, %d\n", obj->doors, obj->seatbelts, obj->lights, obj->r);
            break;
    case 2:                // XML
            fp = fopen("/tmp/carData.xml", "a");
            fprintf(fp, "<car>\n\t<doors> %d </doors>\n\t<seatbelts> %d </seatbelts>\n\t<lights> %d </lights>\n\t<r> %d </r>\n</car>\n", obj->doors, obj->seatbelts, obj->lights, obj->r);
            break;
    case 3:                // JSON
            fp = fopen("/tmp/carData.json", "a");
            fprintf(fp, "{ \"Car\" :{ \"doors \" : %d, \"seatbelts\" : %d, \"lights\" : %d, \"r\" : %d}}\n", obj->doors, obj->seatbelts, obj->lights, obj->r);
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