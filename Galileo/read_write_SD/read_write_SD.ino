/****************************************************************************
 * Copyright (C) 2015 by Daniel Sienkiewicz                                 *
 *                                                                          *
 *   Read_write_SD is free software: you can redistribute it and/or modify it*
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
 * @file read_write_SD.ino
 * @author Daniel Sienkiewicz
 * @date 20 September 2015
 */

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
}

void readSD(char *filename){
    Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  File dataFile = SD.open(filename);
  
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  
  else {
    Serial.println("error opening file");
  } 
}

void writeSD(char *filename, char * data){
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  }
}

void removeSD(char *filename){
  if (SD.exists(filename)) {
    Serial.println("Removing file...");
    SD.remove(filename);
  }
}

void loop() {
  writeSD("log.txt", "dataToFile");
  readSD("log.txt");
  removeSD("log.txt");
}
