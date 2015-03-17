/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSextupole_h
#define BDSSextupole_h 1

#include "globals.hh"

#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"

#include <list>

class BDSSextupole :public BDSMultipole
{
public:
  BDSSextupole(G4String           name,
	       G4double           length,
	       G4double           bDblPrime,
	       BDSBeamPipeInfo    beamPipeInfo,
	       BDSMagnetOuterInfo magnetOuterInfo,
	       BDSTunnelInfo      tunnelInfo);
  ~BDSSextupole(){;};

private:
  G4double itsBDblPrime;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  
};

#endif
