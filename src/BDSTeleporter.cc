#include "globals.hh" //G4 global constants & types
#include "BDSTeleporter.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh" 
#include "BDSMaterials.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "BDSDebug.hh"
#include "BDSMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSTeleporterStepper.hh"
#include "G4ThreeVector.hh"
#include "BDSBeamline.hh"
#include <cmath>
#include "parser/enums.h"

BDSTeleporter::BDSTeleporter(G4String name,
			     G4double length):
  BDSAcceleratorComponent(name,
			  length,
			  0,
			  0,
			  0),
  itsChordFinder(NULL),itsFieldManager(NULL),itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " Constructing Teleporter of length: " 
	 << length/CLHEP::m << " m" << G4endl;
#endif
}

void BDSTeleporter::Build()
{
  BuildBPFieldAndStepper();   // create custom stepper
  BuildBPFieldMgr(itsStepper,itsMagField);  // register it in a manager
  BDSAcceleratorComponent::Build(); // create logical volume and attach manager(stepper)
}

void BDSTeleporter::BuildMarkerLogicalVolume()
{
  itsMarkerLogicalVolume = 
    new G4LogicalVolume(
			new G4Box (itsName+"_solid",
				   BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				   BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				   itsLength/2.0),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName);
  itsMarkerLogicalVolume->SetFieldManager(itsFieldManager,false); // modelled from BDSMultipole.cc
}
  
void BDSTeleporter::BuildBPFieldAndStepper()
{
#ifdef BDSDEBUG
  G4cout << "BDSTeleporter Build Stepper & Field " << G4endl;
#endif
  // set up the magnetic field and stepper
  itsMagField = new BDSMagField(); //Zero magnetic field.
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper  = new BDSTeleporterStepper(itsEqRhs);
}

void BDSTeleporter::BuildBPFieldMgr( G4MagIntegratorStepper* stepper,
    G4MagneticField* field)
{
  //this is all copied from BDSMultipole.cc although names tidied a bit
  itsChordFinder = 
    new G4ChordFinder(field,
    itsLength*0.5/CLHEP::m,
		      stepper);

  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsFieldManager = new G4FieldManager();
  itsFieldManager->SetDetectorField(field);
  itsFieldManager->SetChordFinder(itsChordFinder);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    itsFieldManager->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    itsFieldManager->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    itsFieldManager->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
    itsFieldManager->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
}

void BDSTeleporter::SetVisAttributes()
{
  //make it visible if debug build and invisible otherwise
  itsVisAttributes = new G4VisAttributes(G4Colour(0.852,0.438,0.836,0.5));
#if defined BDSDEBUG
  itsVisAttributes->SetVisibility(true);
#else
  itsVisAttributes->SetVisibility(false);
#endif
}

void CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline)
{
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  G4ThreeVector* lastitemposition   = thebeamline->GetLastPosition();
  G4ThreeVector* firstitemposition  = thebeamline->GetFirstPosition();
  G4ThreeVector  delta              = *lastitemposition/CLHEP::m - *firstitemposition/CLHEP::m;
#ifdef BDSDEBUG
  G4cout << "Calculating Teleporter delta" << G4endl;
  G4cout << "last item position  : " << *lastitemposition/CLHEP::m << G4endl;
  G4cout << "first item position : " << *firstitemposition/CLHEP::m << G4endl;
#endif
  G4cout << "Teleport delta      : " << delta << G4endl;
  BDSGlobalConstants::Instance()->SetTeleporterDelta(delta*CLHEP::m);
  
  // calculate length of teleporter
  // beamline is built along z and sbend deflects in x
  // setting length here ensures that length is always the z difference
  G4double teleporterlength       = fabs(delta.z()*CLHEP::m) - 1e-8*CLHEP::m;
  G4cout << "Calculated teleporter length : " << teleporterlength/CLHEP::m << " m" << G4endl;
  BDSGlobalConstants::Instance()->SetTeleporterLength(teleporterlength);
}




void AddTeleporterToEndOfBeamline(ElementList* beamline_list)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << ": adding teleporter element to end of beamline" << G4endl;
#endif
  //based on void add_sampler in parser.h
  //create basic element with type teleporter and put on end
  struct Element e;
  e.type = _TELEPORTER;
  e.name = "Teleporter";
  e.lst  = NULL; 
  beamline_list->push_back(e);
}

BDSTeleporter::~BDSTeleporter()
{
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
