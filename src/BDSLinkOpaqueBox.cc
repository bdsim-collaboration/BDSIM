/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSAcceleratorComponent.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4types.hh"
#include "G4UserLimits.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <limits>

BDSLinkOpaqueBox::BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
				   G4int indexIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(acceleratorComponentIn),
  index(indexIn)
{

  BDSExtent extent = component->GetExtent();
  G4double gap = 50*CLHEP::cm;
  G4double opaqueBoxThickness = 10*CLHEP::mm;
  auto name = component->GetName();


  auto terminatorBoxOuter = new G4Box(name + "_terminator_box_outer_solid",
			 0.5 * extent.DX() + gap + opaqueBoxThickness,
			 0.5 * extent.DY() + gap + opaqueBoxThickness,
			 0.5 * extent.DZ() + gap + opaqueBoxThickness);

  auto terminatorBoxInner = new G4Box(name + "_terminator_box_inner_solid",
			 0.5 * extent.DX() + gap,
			 0.5 * extent.DY() + gap,
			 0.5 * extent.DZ() + gap);

  auto opaqueBox = new G4SubtractionSolid(name + "_opaque_box_solid",
					  terminatorBoxOuter,
					  terminatorBoxInner);

  auto opaqueBoxLV = new G4LogicalVolume(
      opaqueBox, BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
      name + "_opaque_box_lv");

  auto termUL = new G4UserLimits(std::numeric_limits<double>::max(),
				 std::numeric_limits<double>::max(),
				 std::numeric_limits<double>::max(),
				 0., 0.);
  RegisterUserLimits(termUL);
  opaqueBoxLV->SetUserLimits(termUL);

  
  G4double ls = BDSGlobalConstants::Instance()->LengthSafety();
  containerSolid = new G4Box(name + "_opaque_box_vacuum_solid",
			     0.5 * extent.DX() + gap + opaqueBoxThickness + ls,
		       	     0.5 * extent.DY() + gap + opaqueBoxThickness + ls,
			     0.5 * extent.DZ() + gap + opaqueBoxThickness + ls);

  containerLogicalVolume = new G4LogicalVolume(
      containerSolid, BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
      name + "_container_lv");

  // auto boxPlacement = 
    new G4PVPlacement(nullptr,
					G4ThreeVector(),
					opaqueBoxLV,
					name + "_opaque_box_pv",
					containerLogicalVolume,
					false,
					1,
					true);
  
  // auto collimatorPlacement =
    new G4PVPlacement(nullptr,
					       G4ThreeVector(),
					       component->GetContainerLogicalVolume(),
					       component->GetName() + "_pv",
					       containerLogicalVolume,
					       false,
					       1,
					       true);
  
					      

  outerExtent = BDSExtent(0.5 * extent.DX() + gap + opaqueBoxThickness + ls,
			  0.5 * extent.DY() + gap + opaqueBoxThickness + ls,
			  0.5 * extent.DZ() + gap + opaqueBoxThickness + ls);
    

  // build container geometry.
  // One box minus smaller box to make room for collimator.

  // Make a container of vacuum

  // place component in box.
  // place sampler / output plane in box with index.

  // Update containerSolid and containerLV of BDSGeometryComponent.
  // Update extents of BDSGeometryComponent.
  // register daughters BDSGeometryComponent.


}



BDSExtent BDSLinkOpaqueBox::GetExtent()
{
  return component->GetExtent();
}
