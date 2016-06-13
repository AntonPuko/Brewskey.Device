#ifndef ITap_h
#define ITap_h

#include "application.h"

class ITap {
public:
  virtual String GetId() = 0;
  virtual void AddToFlowCount(uint8_t pulses) = 0;
  virtual void SetAuthToken(String authenticationKey) = 0;
  virtual ~ITap() {}
};

#endif
