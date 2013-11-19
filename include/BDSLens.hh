#ifndef BDSLens_h
#define BDSLens_h 

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

class BDSLens 
{
public:
  BDSLens(G4String name, G4double diameter, G4double rho, G4double centreThickness);
  ~BDSLens();
  G4LogicalVolume* log();
  G4String name();

private:
  void build();
  void visAtt();

  G4double _diameter;
  G4double _rho; //radius of curvature
  G4double _centreThickness;
  G4double _edgeThickness;
  G4String _name;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  G4VisAttributes* _visAtt;
};

#endif
