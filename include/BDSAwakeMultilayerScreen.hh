#ifndef __AWAKE_MULTILAYER_SCREEN_H_
#define __AWAKE_MULTILAYER_SCREEN_H_

#include "BDSMultilayerScreen.hh"

class BDSAwakeMultilayerScreen : public BDSMultilayerScreen{
public:
  BDSAwakeMultilayerScreen(G4String material, G4double thickness, G4double windowScreenGap, G4double dgrain, G4double windowThickness, G4String windowMaterial, G4double mountThickness, G4String mountMaterial, G4double width=1*CLHEP::m);
  virtual ~BDSAwakeMultilayerScreen();
  void surfaces();
  void place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);

private:
  typedef BDSMultilayerScreen super;
  void layers();
  void sampler(G4String name, const char* material="air", G4bool bSampler=true);
  void thinAirLayer();  
  void thinVacuumLayer();  
  void preWindowSampler();
  void postWindowSampler();
  void windowScreenGap();
  void preScreenSampler();
  void postMountSampler();
  void postScreenSampler();
  void windowLayer();
  void backLayer();
  void substrateLayer();
  void mountLayer();
  void binderLayer();
  void backBinderLayer();
  void scintillatorLayer();
  void backScintillatorLayer();
  void frontScintillatorLayer1();
  void frontScintillatorLayer2();
  void frontLayer();
  void reflectiveSurface();
  void roughSurface();
  G4String _material;
  G4double _thickness;
  G4double _windowScreenGap;
  G4double _gapWidth;
  G4double _gapSpacing;
  G4double _dgrain;
  G4double _windowThickness;
  G4String _windowMaterial;
  G4double _mountThickness;
  G4String _mountMaterial;
  G4double _layerThickness;
  G4double _binderLayerThickness;
  G4double _firstLayerThickness;
  G4double _firstBinderLayerThickness;
  G4double _nScintLayers;
  G4double _fillFactor;

  G4int _scintLayerCount;
  G4int _binderLayerCount;
  std::stringstream _ss;
  G4String _binderLayerName;
  G4String _scintLayerName;
  
  void incBinderLayer();
  void incScintLayer();
  
};

#endif
