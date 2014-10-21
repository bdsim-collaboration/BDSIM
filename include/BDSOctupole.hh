/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSOctupole_h
#define BDSOctupole_h 1

#include "globals.hh"
#include "BDSMultipole.hh"

class BDSOctupole :public BDSMultipole
{
public:
  BDSOctupole(G4String aName, G4double aLength,
	      G4double bpRad, G4double FeRad,
	      G4double BTrpPrime, G4double tilt, G4double outR, 
              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
              G4String aTunnelMaterial = "",
	      G4String aMaterial = "");
  ~BDSOctupole();

protected:
  virtual void Build();

private:
  G4double itsBTrpPrime;

  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
};

#endif
