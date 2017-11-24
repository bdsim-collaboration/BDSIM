/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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

BDSShield::BDSShield(G4String         nameIn,
		     G4double         lengthIn,
		     G4double         outerDiameterIn,
		     G4double         xAperIn,
		     G4double         yAperIn,
		     G4Material*      materialIn,
		     BDSBeamPipeInfo* beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "shield", beamPipeInfoIn),
  outerDiameter(outerDiameterIn),
  xAper(xAperIn),
  yAper(yAperIn),
  material(materialIn)
{;}

BDSShield::~BDSShield()
{;}

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
				   chordLength); // extra long for unambiguous subtraction

  G4VSolid* shieldSolid = new G4SubtractionSolid(name+"shield_solid",
						 outerSolid,   // this
						 innerSolid);  // minus this
  

  G4LogicalVolume* shieldLV = new G4LogicalVolume(shieldSolid,
						  material,
						  name+"_shield_lv");
  
  RegisterSolid(outerSolid);
  RegisterSolid(innerSolid);
  RegisterSolid(shieldSolid);
  RegisterLogicalVolume(shieldLV);
  RegisterSensitiveVolume(shieldLV);

  G4VisAttributes* shieldVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("shield"));
  shieldVisAttr->SetVisibility(true);

  shieldLV->SetVisAttributes(shieldVisAttr);

  RegisterVisAttributes(shieldVisAttr);

  G4PVPlacement* shieldPV = new G4PVPlacement(nullptr,
					      G4ThreeVector(),
					      shieldLV,
					      name + "_shield_pv",
					      containerLogicalVolume,
					      false,
					      0,
					      checkOverlaps);

  RegisterPhysicalVolume(shieldPV);
}

void BDSShield::BuildBeamPipe()
{
  // check we should build a beam pipe
  if (!beamPipeInfo)
    {return;}
  
  // check beam pipe fits
  if ((xAper < beamPipeInfo->aper1*2) || (yAper < beamPipeInfo->aper2*2))
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
