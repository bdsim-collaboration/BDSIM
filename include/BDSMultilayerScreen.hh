#ifndef BDSMULTILAYERSCREEN_H
#define BDSMULTILAYERSCREEN_H 

#include <vector>

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4PVPlacement;
class G4VSolid;

class BDSColourWheel;
class BDSScreenLayer;

/** 
 * @brief An accelerator component for diagnostics screens e.g. OTR. Screen inside beam pipe.
 *
 * @author Lawrence Deacon
 */

class BDSMultilayerScreen 
{
public:
  BDSMultilayerScreen(G4TwoVector xysize,
		      G4String    name);
  virtual ~BDSMultilayerScreen();

  inline const G4ThreeVector& GetSize() const {return size;}

  void screenLayer(G4double thickness, G4String material, G4String name, G4int isSampler=0, G4double grooveWidth=0, G4double grooveSpatialFrequency=0);

  void screenLayer(BDSScreenLayer* layer, G4int isSampler=0);

  inline BDSScreenLayer* screenLayer(G4int layer){return screenLayers[layer];}

  BDSScreenLayer* screenLayer(G4String layer);

  BDSScreenLayer* lastLayer();

  inline G4double nLayers()const{return screenLayers.size();}

  void build();
  inline void SetPhys(G4PVPlacement* physIn){phys = physIn;}
  virtual void place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);
  void reflectiveSurface(G4int layer1, G4int layer2);
  void roughSurface(G4int layer1, G4int layer2);

private:
  G4TwoVector xysize;
  G4String name;
  G4ThreeVector size;

  // Geometrical objects:
  G4LogicalVolume* log;
  G4PVPlacement* phys;
  G4VSolid* solid;
  std::vector<BDSScreenLayer*> screenLayers;
  std::vector<G4double> screenLayerZPos;
  void computeDimensions();
  void buildMotherVolume();
  void placeLayers();

  BDSColourWheel* colourWheel;
};

#endif
