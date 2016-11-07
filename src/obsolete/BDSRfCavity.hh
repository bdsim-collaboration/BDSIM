#ifndef BDSRFCAVITY_H
#define BDSRFCAVITY_H

#include "BDSMagnet.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4UniformElectricField.hh"

class BDSBeamPipeInfo;
class BDSMagnetOuterInfo;

class G4ChordFinder;
class G4MagErrorStepper;

class BDSRfCavity: public BDSMagnet
{
public:
  BDSRfCavity(G4String            name,
	      G4double            length,
	      G4double            grad,
	      BDSBeamPipeInfo*    beamPipeInfoIn,
	      BDSMagnetOuterInfo* magnetOuterInfo);
  virtual ~BDSRfCavity();
  
private:
  virtual void BuildVacuumField();
  
  G4double gradient; // longitudinal E field grad in MV / m

  // field related objects:
  G4UniformElectricField* eField;
  G4EqMagElectricField*   equation;
  G4FieldManager*         fieldManager;
  G4MagInt_Driver*        intgrDriver;
  G4MagErrorStepper*      stepper;
  G4ChordFinder*          chordFinder;
};

#endif
