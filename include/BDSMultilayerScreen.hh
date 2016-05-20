#ifndef BDSMULTILAYERSCREEN_H
#define BDSMULTILAYERSCREEN_H 

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4TwoVector.hh"
#include "BDSScreenLayer.hh"
#include "BDSColourWheel.hh"

class BDSMultilayerScreen 
{
public:
  BDSMultilayerScreen(G4TwoVector xysize, G4String name); //X-Y size
  ~BDSMultilayerScreen();
  G4LogicalVolume* log();
  inline G4String name(){return _name;}
  inline G4ThreeVector size(){return _size;}
  void screenLayer(G4double thickness, G4String material, G4String name, G4int isSampler=0, G4double grooveWidth=0, G4double grooveSpatialFrequency=0);
  void screenLayer(BDSScreenLayer* layer, G4int isSampler=0);
  inline BDSScreenLayer* screenLayer(G4int layer){return _screenLayers[layer];}
  BDSScreenLayer* screenLayer(G4String layer);
  BDSScreenLayer* lastLayer();
  inline G4double nLayers(){return _screenLayers.size();}
  void build();
  inline void phys(G4PVPlacement* phys){_phys = phys;}
  inline G4PVPlacement* phys(){return _phys;}
  virtual void place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);
  void reflectiveSurface(G4int layer1, G4int layer2);
  void roughSurface(G4int layer1, G4int layer2);

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

  BDSColourWheel* _colourWheel;
};

#endif
