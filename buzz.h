#ifndef BUZZ_H
#define BUZZ_H

#include "Arduino.h"
#include "timer.h"

class Buzzer {
private:
  int pin;
  bool on = true, finished = true;
  Timer timer;

public:
  Buzzer(int _pin);
  void Stat();
  void Alarm(int ms);
  void Play();
  void Stop();
};

#endif


