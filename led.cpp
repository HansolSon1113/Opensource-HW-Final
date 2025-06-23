#include "led.h"

LED::LED(int _pin) {
  pin = _pin;

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void LED::Reverse() {
  stat = !stat;

  digitalWrite(pin, stat);

  if(++i % 2 == 0)
  {
    finished = true;
    i = 0;
  }
}

void LED::Blink(unsigned long mcs) {
  if(finished) 
  {
    finished = false;
    timer1.Reset();
    timer2.Reset();
  }

  timer1.Invoke([this](){ Reverse(); }, mcs / 2);
  timer2.Invoke([this](){ Reverse(); }, mcs);
}

void LED::On() {
  digitalWrite(pin, HIGH);
}

void LED::Off() {
  digitalWrite(pin, LOW);
}

