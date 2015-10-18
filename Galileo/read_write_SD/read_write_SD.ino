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
    Serial.println("error opening log.txt");
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
