#ifndef __AWAKE_MULTILAYER_SCREEN_H_
#define __AWAKE_MULTILAYER_SCREEN_H_

#include "BDSMultilayerScreen.hh"

class BDSAwakeMultilayerScreen : public BDSMultilayerScreen{
public:
  BDSAwakeMultilayerScreen();
  ~BDSAwakeMultilayerScreen();
private:
  void layers();
  void backingLayer();
  void scintillatorLayer();
  void frontLayer();
  void surfaces();
  void reflectiveSurface();
  G4double _gapWidth;
  G4double _gapSpacing;
};

#endif
