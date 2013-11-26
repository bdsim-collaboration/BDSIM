#ifndef BDSMultilayerScreen_h
#define BDSMultilayerScreen_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerSD.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSScreenLayer.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4TwoVector.hh"
#include "BDSScreenLayer.hh"

class BDSMultilayerScreen 
{
public:
  BDSMultilayerScreen(G4TwoVector xysize, G4String name); //X-Y size
  ~BDSMultilayerScreen();
  G4LogicalVolume* log();
  inline G4String name(){return _name;}
  inline G4ThreeVector size(){return _size;}
  void screenLayer(G4double thickness, G4String material, G4String name, G4double grooveWidth, G4double grooveSpatialFrequency);
  void screenLayer(BDSScreenLayer* layer);
  inline BDSScreenLayer* screenLayer(G4int layer){return _screenLayers[layer];}
  BDSScreenLayer* screenLayer(G4String layer);
  inline G4double nLayers(){return _screenLayers.size();}
  void build();
  inline void phys(G4PVPlacement* phys){_phys = phys;}
  inline G4PVPlacement* phys(){return _phys;}
  virtual void place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);
  void reflectiveSurface(G4int layer1, G4int layer2);

private:
  G4TwoVector _xysize;
  G4String _name;
  G4ThreeVector _size;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4PVPlacement* _phys;
  G4VSolid* _solid;
  std::vector<BDSScreenLayer*> _screenLayers;
  std::vector<G4double> _screenLayerZPos;
  void computeDimensions();
  void buildMotherVolume();
  void placeLayers();

};

#endif
