#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#import "arduino.h"
#include "I2C.h"
#include <stdio.h>
#include <stdlib.h>

extern struct car *audi;

/**
******************************************************************************
* A global car structure                                                     *   
* Described main Car structure                                               *
******************************************************************************
*/
struct car {
 int doors;                                                                  // 5 doors
 int seatbelts;                                                              // 4 seatbelts
 int lights;                                                                 // Status of lights - turned on/offi           
 int r;                                                                      // Status of reverse gear 
 float tempOut;                                                              // The outside temperature 
 float tempIn;                                                               // The inside temperature
 float tempEngine;                                                           // The engine temperature
};

void printObj(struct car * obj);
void checkChangesAnalog(struct car *audi);
void checkChangesDigital();
struct car * readData();
void save(struct car *audi, struct car *tmp);
int readTemp(int portNumber);

#endif
