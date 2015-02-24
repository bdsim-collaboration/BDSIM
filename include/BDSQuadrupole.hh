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
#include "BDSMultipole.hh"

class BDSQuadrupole :public BDSMultipole
{
public:
  BDSQuadrupole(G4String aName, G4double aLength,
		G4double bpRad, G4double FeRad,
		G4double bGrad, G4double tilt, G4double outR, 
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
		 G4String aTunnelMaterial="", G4String aMaterial= "", G4String spec="");
  BDSQuadrupole(G4String        nameIn,
		G4double        lengthIn,
		G4double        bGrad,
		BDSBeamPipeType beamPipeType,
		G4double        aper1,
		G4double        aper2,
		G4double        aper3,
		G4double        aper4,
		G4Material*     vacuumMaterial,
		G4double        beamPipeThickness,
		G4Material*     beamPipeMaterial,
		G4String        outerMaterial="",
		G4String        tunnelMaterial="",
		G4double        tunnelRadius=0,
		G4double        tunnelOffsetX=0);

		
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

  void SetVisAttributes();
};

#endif
