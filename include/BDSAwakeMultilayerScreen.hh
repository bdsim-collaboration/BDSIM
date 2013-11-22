#ifndef __AWAKE_MULTILAYER_SCREEN_H_
#define __AWAKE_MULTILAYER_SCREEN_H_

#include "BDSMultilayerScreen.hh"

class BDSAwakeMultilayerScreen : public BDSMultilayerScreen{
public:
  BDSAwakeMultilayerScreen();
  ~BDSAwakeMultilayerScreen();
private:
  void addLayers();
  void addBackingLayer();
  void addScintillatorLayer();
  void addFrontLayer();
  void addReflectiveSurface();
  G4double _gapWidth;
  G4double _gapSpacing;
};

#endif
