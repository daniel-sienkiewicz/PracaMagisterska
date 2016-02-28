#ifndef _I2C_H_
#define _I2C_H_
#import "arduino.h"
#include <stdio.h>

#define sda      27
#define scl      24
#define pinInt0  14

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
