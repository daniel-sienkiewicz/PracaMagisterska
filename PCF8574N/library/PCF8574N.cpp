/****************************************************************************
 * Copyright (C) 2015 by Daniel Sienkiewicz                                 *
 *                                                                          *
 *   PFC8574N is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   PFC8574N is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with Box. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

/**
 * @file PFC8574N.CPP
 * @author Daniel Sienkiewicz
 * @date 18 October 2015
 */

#include "PCF8574N.h"
#import <Arduino.h>

PCF8574N::PCF8574N(char address, int sda, int scl){
	_address = address;
	_sda = sda;
	_scl = scl;
}

PCF8574N::~PCF8574N(){}

void PCF8574N::init(int sda, int scl){
	pinMode(sda, OUTPUT);
	pinMode(scl, OUTPUT);
	digitalWrite(scl, HIGH);
	digitalWrite(sda, HIGH);
}

int PCF8574N::read8(){
	init(_sda, _scl);
	int m, ack, answer = 0, d = 1;
	digitalWrite(_sda, LOW);
	delay(d);
	digitalWrite(_scl, LOW);
	
	for(m = 0x80; m; m >>= 1){ // address transfer MSB->LSB
		if(_address & m)         
			digitalWrite(_sda,HIGH);
		else
			digitalWrite(_sda,LOW);
        
		digitalWrite(_scl,HIGH); // generate clock pulse
		delay(d);
		digitalWrite(_scl,LOW); 
		delay(d);
	}

   pinMode(_sda,INPUT);
   digitalWrite(_scl,HIGH);
   delay(d);
   
   //Read ACL
   ack = digitalRead(_sda);
   digitalWrite(_scl,LOW);
   delay(d);
   
   for(m = 0x80; m; m>>=1){
		digitalWrite(_scl,HIGH);
		delay(d);
    
		if(digitalRead(_sda)){
			answer |= m;
		}
     
		digitalWrite(_scl, LOW);
		delay(d);
	}
   
	// Generate STOP bit
	pinMode(_sda,OUTPUT);
	digitalWrite(_scl, HIGH);
	delay(d);
	digitalWrite(_sda, HIGH);
	delay(d);
	return answer;
}

void PCF8574N::write8(int data){
	
}