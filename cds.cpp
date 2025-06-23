#include "cds.h"

CDS::CDS(int _pin) {
  pin = _pin;

  pinMode(_pin, INPUT);
}

long CDS::Read() {
  return map(analogRead(pin), 0, 1023, 0, 100);
}