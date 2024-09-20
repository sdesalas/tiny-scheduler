#include "Servo.h"
#include <stdio.h>

Servo::Servo() {}

unsigned char Servo::attach(int pin) {}

unsigned char Servo::attach(int pin, int min, int max) {}

void Servo::detach() {}

void Servo::write(int value) { printf("Servo -> %d\n", value); }

void Servo::writeMicroseconds(int value) {}

int Servo::read() // return the value as degrees
{}

int Servo::readMicroseconds() {}

bool Servo::attached() {}
