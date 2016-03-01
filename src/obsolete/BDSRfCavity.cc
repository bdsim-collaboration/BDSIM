#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSRfCavity.hh"

#include "globals.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4ExplicitEuler.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4UniformElectricField.hh"
#include "G4VPhysicalVolume.hh"

class BDSMagnetOuterInfo;

BDSRfCavity::BDSRfCavity(G4String            name,
			 G4double            length,
			 G4double            grad,
			 BDSBeamPipeInfo*    beamPipeInfo,
			 BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::rfcavity, name, length,
	    beamPipeInfo, magnetOuterInfo, nullptr, 0),
  gradient(grad),
  eField(nullptr),equation(nullptr),fieldManager(nullptr),intgrDriver(nullptr),stepper(nullptr)
{;}

void BDSRfCavity::BuildVacuumField()
{
  G4int nvar = 8;

  // set up the magnetic field and stepper
  G4ThreeVector eFieldVector(0., 0., gradient * CLHEP::megavolt / CLHEP::m);
  eField        = new G4UniformElectricField(eFieldVector);
  equation      = new G4EqMagElectricField(eField);
  fieldManager  = new G4FieldManager();
  fieldManager->SetDetectorField(eField);
  stepper       = new G4ExplicitEuler(equation, nvar);

  G4double minStep = BDSGlobalConstants::Instance()->GetChordStepMinimum();
  
  intgrDriver = new G4MagInt_Driver(minStep,
				    stepper,
				    stepper->GetNumberOfVariables());
  
  chordFinder = new G4ChordFinder(intgrDriver);
  chordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  fieldManager->SetChordFinder(chordFinder);
}

BDSRfCavity::~BDSRfCavity()
{
  delete eField;
  delete equation;
  // must work out what to delete here
  //delete intgrDriver; seems to be deleted by itsChordFinder in BDSMagnet
}
