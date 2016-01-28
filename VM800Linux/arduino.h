#ifndef _arduino_H_
#define _arduino_H_
#include <stdio.h>
#include <stdint.h>
#define HIGH 1
#define LOW 0
#define OUTPUT 0
#define INPUT 1

void digitalWrite(int, int);
void pinMode(int, int);
int digitalRead(int);
float analogRead(int);
void delay(int);

#endif