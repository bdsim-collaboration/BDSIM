#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSDManager.hh"
#include "BDSTerminator.hh"
#include "BDSTerminatorUserLimits.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

BDSTerminator::BDSTerminator(G4String name, G4double length):
  BDSAcceleratorComponent(name, length, 0, "terminator"),
  userLimits(nullptr)
{;}

void BDSTerminator::Build()
{
  BuildContainerLogicalVolume();
}

void BDSTerminator::BuildContainerLogicalVolume()
{
  //Bascially a copy of BDSSampler but with different sensitive detector added
  G4double radius = BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5;
  containerSolid = new G4Box(name + "_container_solid",
			     radius,
			     radius,
			     chordLength * 0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
  // SENSITIVE DETECTOR
  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetTerminatorSD());
  
  // USER LIMITS - the logic of killing particles on last turn
  userLimits = new BDSTerminatorUserLimits(DBL_MAX,DBL_MAX,DBL_MAX,0.,0.);
  RegisterUserLimits(userLimits);
  containerLogicalVolume->SetUserLimits(userLimits);
  //these are default G4UserLimit values so everything will normally be tracked
  //BDSTerminatorUserLimits has the logic inside it to respond to turn number

  // visual attributes
  if (BDSGlobalConstants::Instance()->VisDebug())
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
