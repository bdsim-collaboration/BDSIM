#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTerminator.hh"
#include "BDSTerminatorSD.hh"
#include "BDSTerminatorUserLimits.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"

#include "parser/enums.h"

BDSTerminator::BDSTerminator(G4String name, G4double length):
  BDSAcceleratorComponent(name, length, 0, "terminator")
{;}

void BDSTerminator::Build()
{
  BuildContainerLogicalVolume();
}

void BDSTerminator::BuildContainerLogicalVolume()
{
  //Bascially a copy of BDSSampler but with different sensitive detector added
  G4double radius = BDSGlobalConstants::Instance()->GetSamplerDiameter() * 0.5;
  containerSolid = new G4Box(name + "_container_solid",
			     radius,
			     radius,
			     chordLength * 0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
  // SENSITIVE DETECTOR
  G4SDManager* SDMan    = G4SDManager::GetSDMpointer();
  G4VSensitiveDetector* theTerminator  = new BDSTerminatorSD(name);
  SDMan->AddNewDetector(theTerminator);
  containerLogicalVolume->SetSensitiveDetector(theTerminator);
  
  // USER LIMITS - the logic of killing particles on last turn
  containerLogicalVolume->SetUserLimits(new BDSTerminatorUserLimits(DBL_MAX,DBL_MAX,DBL_MAX,0.,0.));
  //these are default G4UserLimit values so everything will normally be tracked
  //BDSTerminatorUserLimits has the logic inside it to respond to turn number

  // visual attributes
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}

  // register extents with BDSGeometryComponent base class
  SetExtentX(-radius,radius);
  SetExtentY(-radius,radius);
  SetExtentZ(-chordLength*0.5, chordLength*0.5);
}

BDSTerminator::~BDSTerminator()
{;}
