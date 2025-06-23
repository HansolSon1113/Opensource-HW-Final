#include "joystick.h"

Joystick::Joystick(int _xpin, int _ypin) {
  pinMode(_xpin, INPUT);
  pinMode(_xpin, INPUT);

  xpin = _xpin;
  ypin = _ypin;
}

int Joystick::ReadX() {
  int value = analogRead(xpin);
  if (value < LEFT_THRESHOLD)
    return 1;
  else if (value > RIGHT_THRESHOLD) 
    return -1;
  else
    return 0;
}

int Joystick::ReadY() {
  int value = analogRead(ypin);
  if (value < UP_THRESHOLD)
    return 1;
  else if (value > DOWN_THRESHOLD) 
    return -1;
  return 0;
}