/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSCollimatorJaw.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"

#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include <map>

BDSCollimatorJaw::BDSCollimatorJaw(G4String    nameIn,
				   G4double    lengthIn,
				   G4double    horizontalWidthIn,
                   G4double    xApertureIn,
                   G4double    yApertureIn,
                   G4double    jaw1OffsetIn,
                   G4double    jaw2OffsetIn,
				   G4double    xHalfGapIn,
				   G4double    yHalfHeightIn,
				   G4bool      buildLeftJawIn,
				   G4bool      buildRightJawIn,
				   G4Material* collimatorMaterialIn,
				   G4Material* vacuumMaterialIn,
				   G4Colour*   colourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "jcol"),
  jawSolid(nullptr),
  horizontalWidth(horizontalWidthIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  jaw1Offset(jaw1OffsetIn),
  jaw2Offset(jaw2OffsetIn),
  xHalfGap(xHalfGapIn),
  yHalfHeight(yHalfHeightIn),
  buildLeftJaw(buildLeftJawIn),
  buildRightJaw(buildRightJawIn),
  collimatorMaterial(collimatorMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  colour(colourIn)
{
  jawHalfWidth = 0.5 * (0.5*horizontalWidth - lengthSafetyLarge - xHalfGap);
  if (jawHalfWidth < 1e-3) // 1um minimum, could also be negative
    {
      G4cerr << __METHOD_NAME__ << "horizontalWidth insufficient given xsize of jcol" << G4endl;
      exit(1);
    }  
  
  if (!buildLeftJaw || !buildRightJaw)
    {G4cerr << __METHOD_NAME__ << "warning no jaws being built" << G4endl; exit(1);}
}

BDSCollimatorJaw::~BDSCollimatorJaw()
{;}

void BDSCollimatorJaw::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
			     horizontalWidth*0.5,
			     yHalfHeight,
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       vacuumMaterial,
					       name + "_container_lv");
}

void BDSCollimatorJaw::Build()
{
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  jawSolid = new G4Box(name + "_jaw_solid",
		       jawHalfWidth,
		       yHalfHeight - lengthSafetyLarge,
		       0.5*chordLength - 2*lengthSafety);
  RegisterSolid(jawSolid);
  
  G4LogicalVolume* jawLV = new G4LogicalVolume(jawSolid,           // solid
					       collimatorMaterial, // material
					       name + "_jaw_lv");  // name
  
  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  jawLV->SetVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(collimatorVisAttr);
  
  // user limits
  jawLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(jawLV);
  RegisterSensitiveVolume(jawLV);

  if (buildLeftJaw)
    {
      G4ThreeVector leftOffset = G4ThreeVector(xHalfGap + jawHalfWidth, 0, 0);
      G4PVPlacement* leftJPV = new G4PVPlacement(nullptr,                 // rotation
						 leftOffset,              // translation
						 jawLV,                   // logical volume
						 name + "_left_jaw_pv",   // name
						 containerLogicalVolume,  // mother  volume
						 false,		          // no boolean operation
						 0,		          // copy number
						 checkOverlaps);
      RegisterPhysicalVolume(leftJPV);
    }

  if (buildRightJaw)
    {
      G4ThreeVector rightOffset = G4ThreeVector(-xHalfGap - jawHalfWidth, 0, 0);
      G4PVPlacement* rightJPV = new G4PVPlacement(nullptr,                // rotation
						  rightOffset,            // translation
						  jawLV,                  // logical volume
						  name + "_right_jaw_pv", // name
						  containerLogicalVolume, // mother  volume
						  false,		  // no boolean operation
						  0,		          // copy number
						  checkOverlaps);
      RegisterPhysicalVolume(rightJPV);
    }
}
