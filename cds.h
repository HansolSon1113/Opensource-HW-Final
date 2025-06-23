#ifndef CDS_H
#define CDS_H

#include "Arduino.h"

class CDS {
private:
  int pin;
  
public:
  CDS(int _pin);
  long Read();
};

#endif;