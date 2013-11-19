#ifndef BDSCCDChip_h
#define BDSCCDChip_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSCCDPixel.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include<vector>
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"

class BDSCCDChip
{
public:
  BDSCCDChip(G4String aName, G4ThreeVector pixelSize, G4TwoVector nPixels);
  ~BDSCCDChip();

protected:

private:
  void computeDimensions();
  void build();
  void buildMotherVolume();
  void buildPixels();
  
  // Geometrical objects:
  G4VPhysicalVolume* _phys;
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  BDSCCDPixel* _pixel;
  
private:
  const G4TwoVector _nPixels;
  G4ThreeVector _size;
  G4String _name;
};

#endif
