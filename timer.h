#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

class Timer {
private:
  unsigned long prev, current;
  
public:
  Timer() {
    prev = micros();
  }

  void Reset() { prev = micros(); }

  template<typename F>
  void Invoke(F f, unsigned long mcs) {
    unsigned long current = micros();
    if (current - prev >= mcs) {
      prev = current;
      f();
    }
  }
};

#endif



