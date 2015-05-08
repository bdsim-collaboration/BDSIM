/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSOctupole_h
#define BDSOctupole_h 1

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"
#include "G4Material.hh"

class BDSOctupole :public BDSMultipole
{
public:
  BDSOctupole(G4String           name,
	      G4double           length,
	      G4double           bTriplePrime,
	      BDSBeamPipeInfo    beamPipeInfo,
	      BDSMagnetOuterInfo magnetOuterInfo);
  ~BDSOctupole(){;};

protected:
  virtual void Build();
  virtual void BuildBPFieldAndStepper();

private:
  G4double itsBTriplePrime;
};

#endif
