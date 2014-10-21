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

//#define _USE_GEANT4_STEPPER_

#include "globals.hh"

#include "BDSMultipole.hh"

class BDSSolenoid : public BDSMultipole
{
public:
  BDSSolenoid(G4String aName, G4double aLength,
	      G4double bpRad, G4double FeRad,
	      G4double bField, G4double outR, 
              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
              G4String aTunnelMaterial="",
	      G4String aMaterial= "");
  ~BDSSolenoid();

private:
  G4double itsBField;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
};

#endif

