#include "BDSGlobalConstants.hh" 

#include "BDSBeamPipeInfo.hh"
#include "BDSRfCavity.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"

#include "G4MagIntegratorDriver.hh"

#include "G4ExplicitEuler.hh"

BDSRfCavity::BDSRfCavity(G4String        name,
			 G4double        length,
			 G4double        grad,
			 BDSBeamPipeInfo beamPipeInfoIn,
			 G4double        boxSize,
			 G4String        outerMaterial,
			 G4String        tunnelMaterial,
			 G4double        tunnelRadius,
			 G4double        tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfoIn,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
  itsGrad(grad)
{
  itsEField    = NULL;
  fChordFinder = NULL;
  fStepper     = NULL;
  fIntgrDriver = NULL;
}

void BDSRfCavity::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.25,0.25,0.5));
  itsVisAttributes->SetForceSolid(true);
}

void BDSRfCavity::BuildBPFieldAndStepper()
{
  G4int nvar = 8;

  // set up the magnetic field and stepper
  G4ThreeVector Efield(0.,0.,itsGrad * CLHEP::megavolt / CLHEP::m);
  itsEField=new G4UniformElectricField(Efield);

  G4EqMagElectricField* fEquation = new G4EqMagElectricField(itsEField);

  itsBPFieldMgr = new G4FieldManager();
  
  fStepper = new G4ExplicitEuler( fEquation, nvar );
  //itsStepper = new G4ClassicalRK4( fEquation, nvar );

  G4double fMinStep = BDSGlobalConstants::Instance()->GetChordStepMinimum();
 

  itsBPFieldMgr->SetDetectorField(itsEField );

  delete fChordFinder;

  fIntgrDriver = new G4MagInt_Driver(fMinStep,
                                     fStepper,
                                     fStepper->GetNumberOfVariables() );
  
  fChordFinder = new G4ChordFinder(fIntgrDriver);

  fChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsBPFieldMgr->SetChordFinder( fChordFinder );
}

void BDSRfCavity::BuildOuterVolume()
{
  return;
}

