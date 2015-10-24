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
 * @date 24 October 2015
 */

#include "PCF8574N.h"
#import <Arduino.h>

PCF8574N::PCF8574N(char address, int sda, int scl){
	_address = address;
	_sda = sda;
	_scl = scl;

	pinMode(_sda, OUTPUT);
	pinMode(_scl, OUTPUT);
	digitalWrite(_scl, HIGH);
	digitalWrite(_sda, HIGH);
}

PCF8574N::~PCF8574N(){
	digitalWrite(_scl, HIGH);
	digitalWrite(_sda, HIGH);
}

int PCF8574N::read8(void){
	int m, ack, answer = 0, d = 1;
	digitalWrite(_sda, LOW);
	delay(d);
	digitalWrite(_scl, LOW);
	
	for(m = 0x80; m; m >>= 1){ 												// Address transfer MSB->LSB
		if(_address & m)         
			digitalWrite(_sda,HIGH);
		else
			digitalWrite(_sda,LOW);
        
		digitalWrite(_scl,HIGH); 											// Generate clock pulse
		delay(d);
		digitalWrite(_scl,LOW); 
		delay(d);
	}

   pinMode(_sda,INPUT);
   digitalWrite(_scl,HIGH);
   delay(d);
   
   ack = digitalRead(_sda);													// Read ACL
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

	pinMode(_sda,OUTPUT);													// Generate STOP bit
	digitalWrite(_scl, HIGH);
	delay(d);
	digitalWrite(_sda, HIGH);
	delay(d);
	return answer;
}

void PCF8574N::write8(int data){
	
}
