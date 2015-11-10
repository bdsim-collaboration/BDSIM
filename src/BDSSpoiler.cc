#include "BDSGlobalConstants.hh" 
#include "BDSSpoiler.hh"
#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               

BDSSpoiler::BDSSpoiler(G4String      name,
		       G4double      length,
		       G4double      xAperIn,
		       G4double      yAperIn,
		       G4Material*   SpoilerMaterial):
  BDSAcceleratorComponent(name, length, 0, "spoiler"),
  itsPhysiComp(nullptr), itsPhysiComp2(nullptr), itsSolidLogVol(nullptr), 
  itsInnerLogVol(nullptr), itsSpoilerMaterial(SpoilerMaterial),
  xAper(xAperIn),
  yAper(yAperIn)
{;}

void BDSSpoiler::Build()
{
  BDSAcceleratorComponent::Build();
  BuildInnerSpoiler();
}

void BDSSpoiler::BuildMarkerLogicalVolume()
{
  containerSolid = new G4Box(name,
			     BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
			     BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
			     chordLength/2);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSSpoiler::BuildInnerSpoiler()
{
  G4VSolid* solidSolid = new G4Box(name+"_solid",
				   BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				   BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				   chordLength/2);
  RegisterSolid(solidSolid);
  itsSolidLogVol = new G4LogicalVolume(solidSolid,
				       itsSpoilerMaterial,
				       name+"_solid");
  G4VSolid* innerSolid = new G4Box(name+"_inner",
				   xAper,
				   yAper,
				   chordLength/2);
  RegisterSolid(innerSolid);
  itsInnerLogVol = new G4LogicalVolume(innerSolid,
				       BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
				       name+"_inner");
  
  itsPhysiComp2 = new G4PVPlacement(0,                // no rotation
				    (G4ThreeVector)0, // position
				    itsInnerLogVol,   // logical volume
				    name+"_combined", // name
				    itsSolidLogVol,   // its mother  volume
				    false,		// no boolean operation
				    0,                // copy number 
				    checkOverlaps);
  
  RegisterLogicalVolume(itsSolidLogVol);
  RegisterLogicalVolume(itsInnerLogVol);
  SetAcceleratorVacuumLogicalVolume(itsInnerLogVol);
  RegisterPhysicalVolume(itsPhysiComp2);
  
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents())
    {RegisterSensitiveVolume(itsSolidLogVol);}
  
  itsPhysiComp = new G4PVPlacement(0,                      // no rotation
				   (G4ThreeVector)0,       // its position
				   itsSolidLogVol,         // its logical volume
				   name+"_solid",	      // its name
				   containerLogicalVolume, // its mother  volume
				   false,		      // no boolean operation
				   0,                      // copy number  
				   checkOverlaps);
  RegisterPhysicalVolume(itsPhysiComp);
}

BDSSpoiler::~BDSSpoiler()
{;}
