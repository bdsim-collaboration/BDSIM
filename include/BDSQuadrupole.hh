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
#include "BDSBeamPipeInfo.hh"

class BDSQuadrupole :public BDSMultipole
{
public:
  BDSQuadrupole(G4String aName, G4double aLength,
		G4double bpRad, G4double FeRad,
		G4double bGrad, G4double tilt, G4double outR, 
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
		 G4String aTunnelMaterial="", G4String aMaterial= "", G4String spec="");
  BDSQuadrupole(G4String     name,
		G4double     length,
		G4double     bGrad,
		beamPipeInfo beamPipeInfo,
		G4double     boxSize,
		G4String     outerMaterial="",
		G4String     tunnelMaterial="",
		G4double     tunnelRadius=0,
		G4double     tunnelOffsetX=0);
		
  ~BDSQuadrupole(){;}

private:
  G4double itsBGrad;
  
  virtual void Build();

  virtual void BuildOuterLogicalVolume(bool outerMaterialIsVacuum = false);
  virtual void BuildBPFieldAndStepper();
  
  /// quad with poles and pockets
  void BuildStandardOuterLogicalVolume();

  void SetVisAttributes();
};

#endif
