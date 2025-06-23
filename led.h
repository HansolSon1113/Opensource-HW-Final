#ifndef LED_H
#define LED_H

#include "Arduino.h"
#include "timer.h"

class LED {
private:
  int pin, i;
  bool stat = false, finished = true;
  Timer timer1, timer2;

public:
  LED(int _pin);
  void Reverse();
  void Blink(unsigned long mcs);
  void On();
  void Off();
};

#endif


