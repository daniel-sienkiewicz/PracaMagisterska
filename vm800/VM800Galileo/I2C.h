#ifndef _I2C_H_
#define _I2C_H_
#import <Arduino.h>

#define sda      7
#define scl      6
#define pinInt0  2

/**
******************************************************************************
* Analog ports                                                               *
* A0 - temp Out                                                              *
* A1 - temp In                                                               *
* A2 - temp Engine                                                           *
******************************************************************************
*/

int readPCF(char adres);

#endif
