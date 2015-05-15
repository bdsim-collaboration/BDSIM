#ifndef BDSCCDPixel_h
#define BDSCCDPixel_h 

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "BDSCCDPixelSD.hh"

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
  BDSCCDPixelSD* _sensDet;
};

#endif
