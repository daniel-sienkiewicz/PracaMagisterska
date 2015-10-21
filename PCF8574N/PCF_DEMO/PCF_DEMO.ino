#include <PCF8574N.h>

/****************************************************************************
 * Copyright (C) 2015 by Daniel Sienkiewicz                                 *
 *                                                                          *
 *   PCF_DEMO is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   PCF_DEMO is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with Box. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

/**
 * @file PCF_DEMO.ino
 * @author Daniel Sienkiewicz
 * @date 18 October 2015
 */
 
int sda = 8;
int scl = 9;
PCF8574N PCF_41(0x41, sda, scl);
PCF8574N PCF_43(0x43, sda, scl);

void setup(){
  Serial.begin(9600);
  Serial.println("Setup...");
}

void loop(){
   Serial.println(PCF_41.read8());
   Serial.println(PCF_43.read8());
   delay(500);
}
