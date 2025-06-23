#include "Arduino.h"

#ifndef JOYSTICK_H
#define JOYSTICK_H

#define LEFT_THRESHOLD 400
#define RIGHT_THRESHOLD 800
#define UP_THRESHOLD 400
#define DOWN_THRESHOLD 800

class Joystick {
private:
  int xpin, ypin;
  
public:
  Joystick(int _xpin, int _ypin);
  int ReadX();
  int ReadY();
};

#endif;