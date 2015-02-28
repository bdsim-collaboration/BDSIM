/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSOctupole_h
#define BDSOctupole_h 1

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSBeamPipeType.hh"
#include "G4Material.hh"

class BDSOctupole :public BDSMultipole
{
public:
  BDSOctupole(G4String aName, G4double aLength,
	      G4double bpRad, G4double FeRad,
	      G4double BTrpPrime, G4double tilt, G4double outR, 
              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
              G4String aTunnelMaterial = "",
	      G4String aMaterial = "");
  BDSOctupole(G4String        name,
	      G4double        length,
	      G4double        bTriplePrime,
	      BDSBeamPipeType beamPipeType,
	      G4double        aper1,
	      G4double        aper2,
	      G4double        aper3,
	      G4double        aper4,
	      G4Material*     vacuumMaterial,
	      G4double        beamPipeThickness,
	      G4Material*     beamPipeMaterial,
	      G4double        boxSize,
	      G4String        outerMaterial="",
	      G4String        tunnelMaterial="",
	      G4double        tunnelRadius=0,
	      G4double        tunnelOffsetX=0);
  ~BDSOctupole(){;};

protected:
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void SetVisAttributes();

private:
  G4double itsBTriplePrime;
};

#endif
