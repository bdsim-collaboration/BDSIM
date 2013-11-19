#ifndef BDSCCDPixel_h
#define BDSCCDPixel_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerSD.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4TwoVector.hh"

class BDSCCDPixel 
{
public:
  BDSCCDPixel(G4ThreeVector size, G4String name);
  ~BDSCCDPixel();
  G4LogicalVolume* log();
  G4String name();
  G4ThreeVector size();

private:
  void build();
  void visAtt();

  G4ThreeVector _size;
  G4String _name;
  G4String _logName;
  G4String _solidName;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  G4VisAttributes* _visAtt;
};

#endif
