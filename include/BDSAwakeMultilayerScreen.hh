#ifndef __AWAKE_MULTILAYER_SCREEN_H_
#define __AWAKE_MULTILAYER_SCREEN_H_

#include "BDSMultilayerScreen.hh"

class BDSAwakeMultilayerScreen : public BDSMultilayerScreen{
public:
  BDSAwakeMultilayerScreen();
  ~BDSAwakeMultilayerScreen();
  void surfaces();
  void place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);

private:
  typedef BDSMultilayerScreen super;
  void layers();
  void backLayer();
  void substrateLayer();
  void scintillatorLayer();
  void frontLayer();
  void reflectiveSurface();
  G4double _gapWidth;
  G4double _gapSpacing;
  G4double _thickness;
  G4String _material;
 };

#endif
