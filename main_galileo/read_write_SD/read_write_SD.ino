#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
}

void readSD(){
    Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  File dataFile = SD.open("log.txt");
  
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  
  else {
    Serial.println("error opening datalog.txt");
  } 
}

void writeSD(){
  String dataString = "daniel";
  File dataFile = SD.open("log.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
}

void removeSD(){
  if (SD.exists("log.txt")) {
    Serial.println("Removing log.txt...");
    SD.remove("log.txt");
  }
}

void loop() {
  writeSD();
  readSD();
  removeSD();
}
