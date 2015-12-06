#include "globals.hh" //G4 global constants & types
#include "BDSTeleporter.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagField.hh"
#include "BDSTeleporterStepper.hh"

#include "G4Box.hh" 
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ThreeVector.hh"
#include <cmath>

BDSTeleporter::BDSTeleporter(G4String name,
			     G4double length):
  BDSAcceleratorComponent(name, length, 0, "teleporter"),
  itsChordFinder(nullptr),itsFieldManager(nullptr),itsStepper(nullptr),itsMagField(nullptr),itsEqRhs(nullptr)
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
  BDSAcceleratorComponent::Build(); // create container and attach stepper
}

void BDSTeleporter::BuildContainerLogicalVolume()
{
  G4double radius = BDSGlobalConstants::Instance()->GetSamplerDiameter() * 0.5;
  containerSolid = new G4Box(name+"_container_solid",
			     radius,
			     radius,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  containerLogicalVolume->SetFieldManager(itsFieldManager,false); // modelled from BDSMagnet.cc

  // register extents with BDSGeometryComponent base class
  SetExtentX(-radius,radius);
  SetExtentY(-radius,radius);
  SetExtentZ(-chordLength*0.5, chordLength*0.5);
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

void BDSTeleporter::BuildBPFieldMgr(G4MagIntegratorStepper* stepper,
				    G4MagneticField* field)
{
  //this is all copied from BDSMagnet.cc although names tidied a bit
  itsChordFinder = new G4ChordFinder(field,
				     chordLength/CLHEP::m,
				     stepper);

  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsFieldManager = new G4FieldManager();
  itsFieldManager->SetDetectorField(field);
  itsFieldManager->SetChordFinder(itsChordFinder);
  // set limits for field (always non zero, so always set)
  itsFieldManager->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  itsFieldManager->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsFieldManager->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  itsFieldManager->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
}

void BDS::CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline)
{
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  G4ThreeVector lastitemposition   = thebeamline->back()->GetReferencePositionEnd();
  G4ThreeVector firstitemposition  = thebeamline->front()->GetReferencePositionStart();
  G4ThreeVector delta              = lastitemposition - firstitemposition;
  G4cout << "Calculating Teleporter delta" << G4endl;
  G4cout << "Last item end position:       " << lastitemposition  << " mm" << G4endl;
  G4cout << "First item start position:    " << firstitemposition << " mm" << G4endl;
  G4cout << "Teleport delta:               " << delta << " mm" << G4endl;
  BDSGlobalConstants::Instance()->SetTeleporterDelta(delta);
  
  // calculate length of teleporter
  // beamline is built along z and sbend deflects in x
  // setting length here ensures that length is always the z difference
  G4double teleporterLength       = fabs(delta.z());
  G4cout << "Calculated teleporter length: " << teleporterLength << " mm" << G4endl;
  BDSGlobalConstants::Instance()->SetTeleporterLength(teleporterLength);
}

BDSTeleporter::~BDSTeleporter()
{
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
  delete itsChordFinder;
  delete itsFieldManager;
}
