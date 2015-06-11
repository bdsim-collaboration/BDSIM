/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSOCTUPOLE_H
#define BDSOCTUPOLE_H

#include "BDSMagnet.hh"
#include "BDSTiltOffset.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSOctupole: public BDSMagnet
{
public:
  BDSOctupole(G4String           name,
	      G4double           length,
	      G4double           bTriplePrime,
	      BDSBeamPipeInfo*   beamPipeInfo,
	      BDSMagnetOuterInfo magnetOuterInfo,
	      BDSTiltOffset      tiltOffset = BDSTiltOffset());
  ~BDSOctupole(){;};

protected:
  virtual void Build();
  virtual void BuildBPFieldAndStepper();

private:
  G4double itsBTriplePrime;
};

#endif
