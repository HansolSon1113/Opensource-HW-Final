#include "buzz.h"

Buzzer::Buzzer(int _pin) {
  pin = _pin;

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void Buzzer::Stat() {
  on = !on;
}

void Buzzer::Alarm(int micros) {
  if(!on)
  {
    digitalWrite(pin, LOW);
    return;
  }

  if(finished) 
  {
    finished = false;
    timer.Reset();
    tone(pin, 261, micros / 2);
  }

  timer.Invoke([this](){ Stop(); }, micros);
}

void Buzzer::Play() {
  if(!on) return;
  tone(pin, 261);
}

void Buzzer::Stop() {
  noTone(pin);
  finished = true;
}

