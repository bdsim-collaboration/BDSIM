#ifndef __AWAKE_MULTILAYER_SCREEN_H_
#define __AWAKE_MULTILAYER_SCREEN_H_

#include "BDSMultilayerScreen.hh"

class BDSAwakeMultilayerScreen : public BDSMultilayerScreen{
public:
  BDSAwakeMultilayerScreen(G4String material, G4double thickness, G4double dgrain, G4double windowThickness, G4String windowMaterial);
  ~BDSAwakeMultilayerScreen();
  void surfaces();
  void place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);

private:
  typedef BDSMultilayerScreen super;
  void layers();
  void preWindowSampler();
  void windowLayer();
  void backLayer();
  void substrateLayer();
  void binderLayer();
  void backBinderLayer();
  void scintillatorLayer();
  void backScintillatorLayer();
  void frontScintillatorLayer1();
  void frontScintillatorLayer2();
  void frontLayer();
  void reflectiveSurface();
  void roughSurface();
  G4double _gapWidth;
  G4double _gapSpacing;
  G4double _thickness;
  G4double _windowThickness;
  G4double _layerThickness;
  G4double _binderLayerThickness;
  G4double _dgrain;
  G4double _firstLayerThickness;
  G4double _firstBinderLayerThickness;
  G4String _material;
  G4String _windowMaterial;
  G4double _nScintLayers;
  G4double _fillFactor;
 };

#endif
