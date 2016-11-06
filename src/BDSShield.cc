#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSShield.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"

class G4Material;

BDSShield::BDSShield(G4String         name,
		     G4double         length,
		     G4double         outerDiameterIn,
		     G4double         xAperIn,
		     G4double         yAperIn,
		     G4Material*      materialIn,
		     BDSBeamPipeInfo* beamPipeInfoIn):
  BDSAcceleratorComponent(name, length, 0, "shield"),
  outerDiameter(outerDiameterIn),
  xAper(xAperIn),
  yAper(yAperIn),
  material(materialIn),
  beamPipeInfo(beamPipeInfoIn)
{;}

BDSShield::~BDSShield()
{
  delete beamPipeInfo;
}

void BDSShield::Build()
{
  BDSAcceleratorComponent::Build();
  BuildShield();
  BuildBeamPipe();
}

void BDSShield::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
			     outerDiameter*0.5,
			     outerDiameter*0.5,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSShield::BuildShield()
{
  G4VSolid* outerSolid = new G4Box(name+"_outer_solid",
				   outerDiameter*0.5 - lengthSafetyLarge,
				   outerDiameter*0.5 - lengthSafetyLarge,
				   chordLength*0.5 - lengthSafety);
  G4VSolid* innerSolid = new G4Box(name+"_inner_solid",
				   xAper,
				   yAper,
				   chordLength*0.5 - 2*lengthSafety);

  G4VSolid* spoilerSolid = new G4SubtractionSolid(name+"spoiler_solid",
						  outerSolid,   // this
						  innerSolid);  // minus this
  

  G4LogicalVolume* spoilerLV = new G4LogicalVolume(spoilerSolid,
						   material,
						   name+"_spoiler_lv");
  
  RegisterSolid(outerSolid);
  RegisterSolid(innerSolid);
  RegisterSolid(spoilerSolid);
  RegisterLogicalVolume(spoilerLV);
  RegisterSensitiveVolume(spoilerLV);

  G4VisAttributes* spoilerVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("shield"));
  spoilerVisAttr->SetVisibility(true);

  spoilerLV->SetVisAttributes(spoilerVisAttr);

  RegisterVisAttributes(spoilerVisAttr);

  G4PVPlacement* spoilerPV = new G4PVPlacement(nullptr,
					       G4ThreeVector(),
					       spoilerLV,
					       name + "_spoiler_pv",
					       containerLogicalVolume,
					       false,
					       0,
					       checkOverlaps);

  RegisterPhysicalVolume(spoilerPV);
}

void BDSShield::BuildBeamPipe()
{
  // check we should build a beam pipe
  if (!beamPipeInfo)
    {return;}
  
  // check beam pipe fits
  if ((xAper > beamPipeInfo->aper1) || (yAper > beamPipeInfo->aper2))
    {
      G4cout << "Shield will not fit around beam pipe - not building beam pipe!" << G4endl;
      return;
    }
  
  // construct and place beam pipe
  auto bp = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
							   chordLength - lengthSafety,
							   beamPipeInfo);
  RegisterDaughter(bp);

  G4PVPlacement* bpPV = new G4PVPlacement(nullptr,
					  G4ThreeVector(),
					  bp->GetContainerLogicalVolume(),
					  name+"_beampipe_pv",
					  containerLogicalVolume,
					  false,
					  0,
					  checkOverlaps);

  RegisterPhysicalVolume(bpPV);
}
