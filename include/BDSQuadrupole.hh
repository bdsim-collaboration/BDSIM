//  
//   BDSIM, (C) 2001-2006 
//   
//   version 0.3
//  
//
//
//   Quadrupole class
//
//
//   History
//
//     21 Nov 2006 by Agapov,  v.0.3
//     22 Mar 2005 by Agapov, Carter,  v.0.2
//     x  x   2002 by Blair
//
//

#ifndef __BDSQUADRUPOLE_H
#define __BDSQUADRUPOLE_H

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "BDSQuadStepper.hh"

#include "G4FieldManager.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSQuadMagField.hh"

class BDSQuadrupole :public BDSMultipole
{
public:
  BDSQuadrupole(G4String aName, G4double aLength,
		G4double bpRad, G4double FeRad,
		G4double bGrad, G4double tilt, G4double outR, 
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
		 G4String aTunnelMaterial="", G4String aMaterial= "", G4String spec="");
  ~BDSQuadrupole();

private:
  G4double itsBGrad;
  
  virtual void Build();

  virtual void BuildOuterLogicalVolume(bool OuterMaterialIsVacuum = false);
  virtual void BuildBPFieldAndStepper();
  
  /// quad with poles and pockets
  void BuildStandardOuterLogicalVolume();
  /// cylinder
  void BuildCylindricalOuterLogicalVolume();

  virtual G4VisAttributes* SetVisAttributes();
};

#endif
