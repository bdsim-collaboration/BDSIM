#include "BDSCavityRF.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"

#include "globals.hh"
#include "G4Material.hh"
#include "BDSPillBoxField.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ElectroMagneticField.hh"
#include "G4EqMagElectricField.hh"
#include "G4ClassicalRK4.hh"

BDSCavityRF::BDSCavityRF(G4String name, 
			 G4double length,
			 G4String type,  //??
			 G4Material* cavityMaterialIn,
			 G4Material* vacuumMaterialIn,
			 G4double cavityRadiusIn, //lrgst radial distance from z axs
			 G4double irisRadiusIn, //radius at ends (assuming equal..)
			 G4double thicknessIn,
			 G4double frequencyIn,
			 G4double phaseIn,
			 G4String cavityModelIn
			 ):
  BDSCavity(name, length, type, cavityMaterialIn, vacuumMaterialIn, cavityRadiusIn, irisRadiusIn, thicknessIn, cavityModelIn)
{
  frequency = 100*CLHEP::megahertz;
  phase = phaseIn;
}

 void BDSCavityRF::BuildField()
{
  G4double eFieldMax = - 160 * CLHEP::megavolt / CLHEP::m; //--------------REMOVE HARDCODED @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  frequency = 100*CLHEP::megahertz;
  itsField = new BDSPillBoxField(eFieldMax,
				 cavityRadius,
				 frequency,
				 phase
				 );
 }

void BDSCavityRF::AttachField()
{
  G4EqMagElectricField* itsEquation = new G4EqMagElectricField(itsField); //Create field
  itsStepper = new G4ClassicalRK4(itsEquation, nvar); //Create stepper

  itsIntgrDriver = new G4MagInt_Driver(BDSGlobalConstants::Instance()->GetChordStepMinimum(), //minimum step
				       itsStepper, //Stepper
				       itsStepper->GetNumberOfVariables() ); //nvar

  itsChordFinder = new G4ChordFinder(itsIntgrDriver); //EM fields need to use integrator driver constructor.
  itsFieldManager = new G4FieldManager();                      //Create field manager

  
  itsFieldManager->SetDetectorField(itsField);               //Set field in field mnager
  itsFieldManager ->SetChordFinder(itsChordFinder);       //set chord finder in manager
  
  //Defining the chord finder parameters.
  itsFieldManager ->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  itsFieldManager ->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsFieldManager ->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  itsFieldManager ->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  
  G4bool allLocal = true ; //using "true" makes it push the field to all existing daugher volumes (and their daughters and so on) -- unless a daughter has its own field manager.

  vacuumLV->SetFieldManager(itsFieldManager,allLocal);

  // std::cout << "field attached" << std::endl;
  
}

void BDSCavityRF::Build()
{
  if (type=="elliptical") {
    BuildEllipticalCavityGeometry();
  } else if (type=="rectangular" || type=="pillbox") {
    BuildPillBoxCavityGeometry();
  } else {
    std::cout << "type is not known: " << type << std::endl;
    exit(1);
  }
  BDSAcceleratorComponent::Build();
  BuildField();
  AttachField();
  PlaceComponents();
}
