/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSDecapole_h
#define BDSDecapole_h 1

#include "globals.hh"

#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSTiltOffset.hh"

#include <list>

class BDSDecapole: public BDSMultipole
{
public:
  BDSDecapole(G4String           name,
	      G4double           length,
	      G4double           bQuadPrime,
	      BDSBeamPipeInfo*   beamPipeInfo,
	      BDSMagnetOuterInfo magnetOuterInfo,
	      BDSTiltOffset      tiltOffset = BDSTiltOffset());
  ~BDSDecapole(){;};

protected:
  virtual void Build();

private:
  G4double itsBQuadPrime;

  virtual void BuildBPFieldAndStepper();
};

#endif
