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
  BDSQuadrupole(G4String           name,
		G4double           length,
		G4double           bGrad,
		BDSBeamPipeInfo    beamPipeInfo,
		BDSMagnetOuterInfo magnetOuterInfo);
		
  ~BDSQuadrupole(){;}

private:
  G4double itsBGrad;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
};

#endif
