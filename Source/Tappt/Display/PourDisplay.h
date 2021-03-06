#ifndef PourDisplay_h
#define PourDisplay_h

#include "ITick.h"
#include "Tap.h"
#include "Display.h"

class PourDisplay: public ITick  {
public:
  PourDisplay(Display* display);

  void Setup(Tap* taps, int tapCount);
  virtual int Tick();
private:
  void SetEmptySlotForTap(int tapId);


  Display* display;
  Tap* taps;
  int tapCount;

  int currentPouringTaps[4] = {-1, -1, -1, -1};
  String currentDisplays[4];
};

#endif
