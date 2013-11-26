#ifndef BDSCCDCamera_h
#define BDSCCDCamera_h 

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
#include "BDSCCDChip.hh"
#include "BDSLens.hh"

class BDSCCDCamera 
{
public:
  BDSCCDCamera();
  ~BDSCCDCamera();
  G4LogicalVolume* log();
  G4String name();
  inline G4ThreeVector size(){return _size;}
  inline void phys(G4PVPlacement* phys){_phys=phys;}
  inline G4PVPlacement* phys(){return _phys;}

private:
  void defaultDimensions();
  void build();
  void buildMotherVolume();
  void buildCCDChip();
  void buildObjectLens();
  void buildImageLens();
  void visAtt();
  void placeComponents();
  void placeCCDChip();
  void placeObjectLens();
  void placeImageLens();

  G4ThreeVector _size;
  G4String _name;
  G4String _logName;
  G4String _solidName;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  G4VisAttributes* _visAtt;
  BDSCCDChip* _ccdChip;
  BDSLens* _objectLens;
  BDSLens* _imageLens;
  G4PVPlacement* _phys;
};

#endif
