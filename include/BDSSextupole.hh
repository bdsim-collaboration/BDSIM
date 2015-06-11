/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSEXTUPOLE_H
#define BDSSEXTUPOLE_H

#include "globals.hh"

#include "BDSMagnet.hh"
#include "BDSTiltOffset.hh"

#include <list>

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSextupole: public BDSMagnet
{
public:
  BDSSextupole(G4String           name,
	       G4double           length,
	       G4double           bDblPrime,
	       BDSBeamPipeInfo*   beamPipeInfo,
	       BDSMagnetOuterInfo magnetOuterInfo,
	       BDSTiltOffset      tiltOffset = BDSTiltOffset());
  ~BDSSextupole(){;};

private:
  G4double itsBDblPrime;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  
};

#endif
