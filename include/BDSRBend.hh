/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 18.10.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   IA: 12.10.05 , modified
*/

#ifndef BDSRBend_h
#define BDSRBend_h 

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "BDSDipoleStepper.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSSbendMagField.hh"
#include "G4Mag_EqRhs.hh"

class BDSRBend :public BDSMultipole
{
public:
  BDSRBend(G4String aName, G4double aLength,
		G4double bpRad, G4double FeRad,
		G4double bField, G4double angle, G4double outR,
           std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
           G4double tilt = 0, G4double bGrad=0, G4String aTunnelMaterial="", G4String aMaterial = "");
  ~BDSRBend();

private:
  G4double itsBField;
  G4double itsBGrad;
  G4double itsMagFieldLength;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildMarkerLogicalVolume();
  virtual void BuildBeampipe(G4String materialName="");
  virtual void BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);

  G4VisAttributes* SetVisAttributes();
  G4Trd* markerSolidVolume;
  G4Trd* rbendRectangleSolidVolume;
  G4LogicalVolume* rbendRectangleLogicalVolume;
  G4LogicalVolume* middleBeampipeLogicalVolume;
  G4LogicalVolume* middleInnerBPLogicalVolume;
  G4LogicalVolume* endsBeampipeLogicalVolume;
  G4LogicalVolume* endsInnerBPLogicalVolume;
  G4UserLimits* endsBeampipeUserLimits;
  G4UserLimits* endsInnerBeampipeUserLimits;
  G4VisAttributes* innerBeampipeVisAtt;
  G4VisAttributes* beampipeVisAtt;

};

#endif
