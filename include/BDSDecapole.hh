/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSDecapole_h
#define BDSDecapole_h 1

#include "globals.hh"

#include "G4VisAttributes.hh"

#include "BDSMultipole.hh"

class BDSDecapole :public BDSMultipole
{
public:
  BDSDecapole(G4String aName, G4double aLength,
	      G4double bpRad, G4double FeRad,
	      G4double BQuadPrime, G4double tilt, G4double outR,
              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
              G4String aTunnelMaterial = "",
	      G4String aMaterial = "");
  ~BDSDecapole();

protected:
  virtual void Build();

private:
  G4double itsBQuadPrime;

  virtual void BuildBPFieldAndStepper();

  virtual G4VisAttributes* SetVisAttributes();
};

#endif
