#ifndef _FT800api_H_
#define _FT800api_H_
#include "FT800.h"
#include "simulator.h"
#import "arduino.h"
#include <stdio.h>
#include <string.h>

extern unsigned int cmdOffset;
extern unsigned int cmdBufferRd;
extern unsigned int cmdBufferWr;
extern struct car *audi;

void initScreen();
void mainScreen();
void smartMirrorScreen();
void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale);
void button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str);
void text( int16_t x,  int16_t y, int16_t font, uint16_t options, const char* str);
void line(unsigned long color, unsigned long line_x1, unsigned long line_y1, unsigned long line_x2, unsigned long line_y2, unsigned long width);
void dot(unsigned long color, unsigned int point_size, unsigned long point_x, unsigned long point_y);
void start(unsigned long color);
void number( int16_t x,  int16_t y, int16_t font, uint16_t options, int32_t value);
void show();
void autko();
#endif
