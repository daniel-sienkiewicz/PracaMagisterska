/**
 * @file I2C.h
 * @author Daniel Sienkiewicz
 * @date 28 February 2016
 * @brief File containing declarations of function to read data with using I2C protocol.
 */

#ifndef _I2C_H_
#define _I2C_H_
#import <Arduino.h>

#define sda      7 /**< SDA port number */
#define scl      6 /**< SCL port number */
#define pinInt0  2 /**< Interrput port number*/

/**
******************************************************************************
* @details        Reading value from PCF8574N I/O Expander                   *
* @param          adres The address of PCF8574N I/O Expander                 *
* @return         Value from the specified PCF8574N I/O Expander             *
******************************************************************************
*/
int readPCF(char adres);

#endif