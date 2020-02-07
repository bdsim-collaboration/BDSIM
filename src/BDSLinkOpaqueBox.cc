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
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <limits>

BDSLinkOpaqueBox::BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
				   G4double tiltIn,
				   G4int indexIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(acceleratorComponentIn),
  index(indexIn)
{
  BDSExtent extent = component->GetExtent();
  extent = extent.Tilted(tiltIn); // apply tilt
  const G4double gap                = 10 * CLHEP::cm;
  const G4double opaqueBoxThickness = 10 * CLHEP::mm;
  G4String name = component->GetName();

  G4Box* terminatorBoxOuter = new G4Box(name + "_terminator_box_outer_solid",
					0.5 * extent.DX() + gap + opaqueBoxThickness,
					0.5 * extent.DY() + gap + opaqueBoxThickness,
					0.5 * extent.DZ() + gap + opaqueBoxThickness);
  RegisterSolid(terminatorBoxOuter);
  G4Box* terminatorBoxInner = new G4Box(name + "_terminator_box_inner_solid",
					0.5 * extent.DX() + gap,
					0.5 * extent.DY() + gap,
					0.5 * extent.DZ() + gap);
  RegisterSolid(terminatorBoxInner);
  G4SubtractionSolid* opaqueBox = new G4SubtractionSolid(name + "_opaque_box_solid",
							 terminatorBoxOuter,
							 terminatorBoxInner);
  RegisterSolid(opaqueBox);
  G4LogicalVolume* opaqueBoxLV = new G4LogicalVolume(opaqueBox,
						     BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
						     name + "_opaque_box_lv");
  RegisterLogicalVolume(opaqueBoxLV);

  G4UserLimits* termUL = new G4UserLimits();
  termUL->SetUserMinEkine(std::numeric_limits<double>::max());
  RegisterUserLimits(termUL);
  opaqueBoxLV->SetUserLimits(termUL);
  
  G4VisAttributes* obVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("opaquebox"));
  obVis->SetVisibility(true);
  opaqueBoxLV->SetVisAttributes(obVis);
  RegisterVisAttributes(obVis);
  
  G4double ls = BDSGlobalConstants::Instance()->LengthSafety();
  G4double margin = gap + opaqueBoxThickness + ls;
  containerSolid = new G4Box(name + "_opaque_box_vacuum_solid",
			                 0.5 * extent.DX() + margin,
		       	             0.5 * extent.DY() + margin,
			                 0.5 * extent.DZ() + margin);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
					       name + "_container_lv");
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());

  G4RotationMatrix* rm = new G4RotationMatrix();
  rm->rotateZ(tiltIn);
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
  new G4PVPlacement(rm,
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

  G4TwoVector xy = (component->Sagitta(),0);
  xy.rotate(tiltIn);
  if (BDS::IsFinite(component->GetAngle()))
    {rm->rotate(-0.5 * component->GetAngle(), G4ThreeVector(0,1,0));}
  offsetToStart = G4ThreeVector(xy.x(), xy.y(), -0.5*component->GetChordLength());
  transformToStart = G4Transform3D(rm->inverse(), offsetToStart);



  // build container geometry.
  // One box minus smaller box to make room for collimator.

  // Make a container of vacuum

  // place component in box.
  // place sampler / output plane in box with index.

  // Update containerSolid and containerLV of BDSGeometryComponent.
  // Update extents of BDSGeometryComponent.
  // register daughters BDSGeometryComponent.
}
