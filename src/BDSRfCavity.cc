#include "BDSGlobalConstants.hh" 

#include "BDSBeamPipeInfo.hh"
#include "BDSRfCavity.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include "G4MagIntegratorDriver.hh"

#include "G4ExplicitEuler.hh"

BDSRfCavity::BDSRfCavity(G4String           name,
			 G4double           length,
			 G4double           grad,
			 BDSBeamPipeInfo*   beamPipeInfo,
			 BDSMagnetOuterInfo magnetOuterInfo):
  BDSMagnet(BDSMagnetType::rfcavity, name, length,
	    beamPipeInfo, magnetOuterInfo),
  gradient(grad)
{
  eField      = NULL;
  equation    = NULL;
  intgrDriver = NULL;
}

void BDSRfCavity::BuildBPFieldMgr()
{;}

void BDSRfCavity::BuildBPFieldAndStepper()
{
  G4int nvar = 8;

  // set up the magnetic field and stepper
  G4ThreeVector eFieldVector(0., 0., gradient * CLHEP::megavolt / CLHEP::m);
  eField        = new G4UniformElectricField(eFieldVector);
  equation      = new G4EqMagElectricField(eField);
  itsBPFieldMgr = new G4FieldManager();
  itsBPFieldMgr->SetDetectorField(eField);
  itsStepper    = new G4ExplicitEuler(equation, nvar);

  G4double minStep = BDSGlobalConstants::Instance()->GetChordStepMinimum();
  
  intgrDriver = new G4MagInt_Driver(minStep,
				    itsStepper,
				    itsStepper->GetNumberOfVariables() );
  
  itsChordFinder = new G4ChordFinder(intgrDriver);
  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsBPFieldMgr->SetChordFinder(itsChordFinder);
}

BDSRfCavity::~BDSRfCavity()
{
  delete eField;
  delete equation;
  delete intgrDriver;
}
