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


#include "G4VisAttributes.hh"
#include "BDSMultipole.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UniformMagField.hh"
#include "BDSSolenoidStepper.hh"
#include "BDSSolenoidMagField.hh"

class G4Mag_UsualEqRhs;
class G4UniformMagField;
class BDSSolenoidStepper;
class BDSSolenoidMagField;

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

  void BuildBPFieldAndStepper();

  G4VisAttributes* SetVisAttributes();

  // field related objects:
#ifdef _USE_GEANT4_STEPPER_
  BDSSolenoidMagField* itsMagField;
  G4MagIntegratorStepper* itsStepper;
#else
  G4UniformMagField* itsMagField;
  BDSSolenoidStepper* itsStepper;
#endif
  G4Mag_UsualEqRhs* itsEqRhs;
};

#endif

