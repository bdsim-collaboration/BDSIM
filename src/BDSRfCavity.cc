#include "BDSGlobalConstants.hh" 

#include "BDSRfCavity.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"

#include "G4MagIntegratorDriver.hh"

#include "G4ExplicitEuler.hh"

//============================================================

BDSRfCavity::BDSRfCavity (G4String aName,G4double aLength, G4double bpRad, 
			  G4double grad, G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName ,aLength, bpRad, bpRad, aTunnelMaterial, aMaterial),
  itsEField(NULL),fChordFinder(NULL),fStepper(NULL),fIntgrDriver(NULL),fieldManager(NULL)
{
  itsGrad = grad;
}

void BDSRfCavity::Build()
{
  BDSMultipole::Build();
  itsInnerBPLogicalVolume->SetFieldManager(fieldManager,false);
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

  fieldManager = new G4FieldManager();
  
  fStepper = new G4ExplicitEuler( fEquation, nvar );
  //itsStepper = new G4ClassicalRK4( fEquation, nvar );

  G4double fMinStep = BDSGlobalConstants::Instance()->GetChordStepMinimum();
 

  fieldManager->SetDetectorField(itsEField );

  delete fChordFinder;

  fIntgrDriver = new G4MagInt_Driver(fMinStep,
                                     fStepper,
                                     fStepper->GetNumberOfVariables() );
  
  fChordFinder = new G4ChordFinder(fIntgrDriver);

  fChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  fieldManager->SetChordFinder( fChordFinder );
}


BDSRfCavity::~BDSRfCavity()
{
}
