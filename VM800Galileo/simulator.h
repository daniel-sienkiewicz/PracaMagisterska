/**
 * @file simulator.h
 * @author Daniel Sienkiewicz
 * @date 28 February 2016
 * @brief File containing declarations of all functions required to communication with car simulator.
 */

#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#import <Arduino.h>
#include "I2C.h"
#include <stdio.h>
#include "FT800api.h"

extern struct car *audi;
extern int dataFormat;
extern int saveData;
extern short int screenNR;

/**
******************************************************************************
* Analog ports                                                               *
* A0 - temp Out                                                              *
* A1 - temp In                                                               *
* A2 - temp Engine                                                           *
******************************************************************************
*/

/**
******************************************************************************
* @details 			A global car structure                    				 *   
******************************************************************************
*/
struct car {
 int doors;		/**< status of doors in car. 1 - open, 0 closed */
 int seatbelts;		/**< status of seatbelts in car. 1 - open, 0 - closed */
 int lights; 		/**< status of lights. 1 -turn on, 0 - turn off */          
 int r; 		/**< statu of reverse gear */
 float tempOut;		/**< temperature outside */
 float tempIn; 		/**< temperature inside */
 float tempEngine;	/**< temperature engine */
};

/**
******************************************************************************
* @details        Debug function to print car structure on a serial monitor  *
*                 console and to log file on SD car                          *
* @param          Car struct to print and save with selected format into file*
******************************************************************************
*/
void printObj(struct car * obj, char *d);

/**
******************************************************************************
* @details        Check if sth on analog ports was changed                  *
******************************************************************************
*/
void checkChangesAnalog(struct car *audi);

/**
******************************************************************************
* @details        Check if sth on digital ports was changed                  *
******************************************************************************
*/
void checkChangesDigital();

/**
******************************************************************************
* @details        Reading data about car status                              *
******************************************************************************
*/
struct car * readData();

/**
******************************************************************************
* @details        Copying data function from temporary to main struct        *
* @param          *audi, *tmp Structures to and from which data are copied   *
******************************************************************************
*/
void save(struct car *audi, struct car *tmp);

/**
******************************************************************************
* @details        Reading value from analog ports (temperatures)             *
* @param          portNumber The number of the analog input pin to read      *
* @return         Value from the specified analog pin                        *
******************************************************************************
*/
int readTemp(int portNumber);

#endif
