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
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "BDSQuadStepper.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
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

  void SynchRescale(G4double factor);

protected:

private:
  G4double itsBGrad;

  void BuildOuterLogicalVolume();
  void BuildBPFieldAndStepper();
  void BuildCylindricalOuterLogicalVolume();

  G4VisAttributes* SetVisAttributes();

  // field related objects:
  BDSQuadStepper* itsStepper;
  BDSQuadMagField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;

};

#endif
