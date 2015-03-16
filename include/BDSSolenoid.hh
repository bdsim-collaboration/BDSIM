//  
//   BDSIM, (C) 2001-2007
//   
//   version 0.4
//  
//
//
//   Solenoid Class
//
//
//   History
//
//     21 Oct 2007 by Marchiori, v.0.4
//

#ifndef __BDSSOLENOID_H
#define __BDSSOLENOID_H

#include "globals.hh"

#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"

#include <list>

class BDSSolenoid : public BDSMultipole
{
public:
  BDSSolenoid(G4String        name,
	      G4double        length,
	      G4double        bField,
	      BDSBeamPipeInfo beamPipeInfoIn,
	      G4double        boxSize,
	      G4String        outerMaterial="",
	      G4String        tunnelMaterial="",
	      G4double        tunnelRadius=0,
	      G4double        tunnelOffsetX=0);
  ~BDSSolenoid(){;};

private:
  G4double itsBField;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
  virtual void BuildOuterVolume();
};

#endif

