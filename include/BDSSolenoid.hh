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

#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMultipole.hh"
#include "BDSTunnelInfo.hh"

#include <list>

class BDSSolenoid : public BDSMultipole
{
public:
  BDSSolenoid(G4String           name,
	      G4double           length,
	      G4double           bField,
	      BDSBeamPipeInfo    beamPipeInfo,
	      BDSMagnetOuterInfo magnetOuterInfo);
  ~BDSSolenoid(){;};

private:
  G4double itsBField;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
};

#endif

