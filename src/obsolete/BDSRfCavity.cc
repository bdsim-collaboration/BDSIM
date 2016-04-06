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

struct BDSMagnetOuterInfo;

BDSRfCavity::BDSRfCavity(G4String            name,
			 G4double            length,
			 G4double            grad,
			 BDSBeamPipeInfo*    beamPipeInfo,
			 BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::rfcavity, name, length,
	    beamPipeInfo, magnetOuterInfo),
  gradient(grad)
{
  eField      = nullptr;
  equation    = nullptr;
  intgrDriver = nullptr;
}

void BDSRfCavity::BuildBPFieldMgr()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "empty implementation due to e field instead of B" << G4endl;
#endif
}

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

  G4double minStep = BDSGlobalConstants::Instance()->ChordStepMinimum();
  
  intgrDriver = new G4MagInt_Driver(minStep,
				    itsStepper,
				    itsStepper->GetNumberOfVariables() );
  
  itsChordFinder = new G4ChordFinder(intgrDriver);
  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->DeltaChord());
  itsBPFieldMgr->SetChordFinder(itsChordFinder);
}

BDSRfCavity::~BDSRfCavity()
{
  delete eField;
  delete equation;
  //delete intgrDriver; seems to be deleted by itsChordFinder in BDSMagnet
}
