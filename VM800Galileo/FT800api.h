/**
 * @file FT800api.h
 * @author Daniel Sienkiewicz
 * @date 28 February 2016
 * @brief File containing declarations of all API functions for VM800.
 */

#ifndef _FT800api_H_
#define _FT800api_H_
#include "FT800.h"
#include "simulator.h"
#import <Arduino.h>

extern unsigned int cmdOffset;
extern unsigned int cmdBufferRd;
extern unsigned int cmdBufferWr;
extern struct car *audi;
extern int timeR;

/**
******************************************************************************
* @details Function showing init screen durig main screen is loading         *
******************************************************************************
*/
void initScreen();

/**
******************************************************************************
* @details Function showing options screen                                   *
******************************************************************************
*/
void opctionsScreen();

/**
******************************************************************************
* @details Function showing main screen                                      *
******************************************************************************
*/
void mainScreen();

/**
******************************************************************************
* @details Function showing smart mirror screen                              *
******************************************************************************
*/
void smartMirrorScreen();

/**
******************************************************************************
* @details Function which draw a spinner on the screen                       *
* @param x x-coordinate on the screen                                        *
* @param y y-coordinate on the screen                                        *
* @param style look swcreen                                                  *
* @param scale size of spinner                                               *
******************************************************************************
*/
void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale);

/**
******************************************************************************
* @details Function which draw a button on the screen                        *
* @param x x-coordinate on the screen                                        *
* @param y y-coordinate on the screen                                        *
* @param w width for the button                                              *
* @param h height for the button                                             *
* @param font font fort the button text                                      *
* @param options options for the button                                      *
* @param str text to draw inside button                                      *
******************************************************************************
*/
void button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str);

/**
******************************************************************************
* @details Function which draw a text on the screen                          *
* @param x x-coordinate on the screen                                        *
* @param y y-coordinate on the screen                                        *
* @param font font for the text                                              *
* @param options options to set for the text                                 *
* @param str text to draw on the screen                                      *
******************************************************************************
*/
void text( int16_t x,  int16_t y, int16_t font, uint16_t options, const char* str);

/**
******************************************************************************
* @details Function which draw a line on the screen                          *
* @param color line color                                                    *
* @param line_x1 x-coordinate for the beginning of the first end             *
* @param line_y1 y-coordinate for the beginning of the first end             *
* @param line_x2 x-coordinate for the beginning of the second end            *
* @param line_y2 y-coordinate for the beginning of the second end            *
* @param width line width                                                    *
******************************************************************************
*/
void line(unsigned long color, unsigned long line_x1, unsigned long line_y1, unsigned long line_x2, unsigned long line_y2, unsigned long width);

/**
******************************************************************************
* @details Function which draw a dot on the screen                           *
* @param color dot color                                                     *
* @param point_size size for the dot                                         *
* @param point_x x-coordinate for the dot                                    *
* @param point_y y-coordinate for the dot                                    *
******************************************************************************
*/
void dot(unsigned long color, unsigned int point_size, unsigned long point_x, unsigned long point_y);

/**
******************************************************************************
* @details Function which start inicjalize new screen                        *
* @param color backgroud color                                               *
******************************************************************************
*/
void start(unsigned long color);

/**
******************************************************************************
* @details Function which draw a number on the screen                        *
* @param x x-coordinate on the screen                                        *
* @param y y-coordinate on the screen                                        *
* @param font font for the number                                            *
* @param options options to set for the number                               *
* @param value value to draw on the screen                                   *
******************************************************************************
*/
void number(int16_t x,  int16_t y, int16_t font, uint16_t options, int32_t value);

/**
******************************************************************************
* @details Function showing prerared screen from buffor                      *
******************************************************************************
*/
void show();

/**
******************************************************************************
* @details Function which calibrate screen                                   *
******************************************************************************
*/
void calibrate();

/**
******************************************************************************
* @details Function drawing car with proper attributes                       *
******************************************************************************
*/
void autko();

#endif
