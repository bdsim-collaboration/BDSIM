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
#include "BDSMaterials.hh"
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

  if (BDS::IsFinite(xAperture))
	{
      if (jaw1Offset > xAperture)
		{
		  G4cerr << __METHOD_NAME__ << "jcol jaw offset cannot be greater than the aperture half-size." << G4endl;
		  exit(1);
		}
	}
  if (BDS::IsFinite(yAperture))
	{
	  if (jaw1Offset > yAperture)
		{
		  G4cerr << __METHOD_NAME__ << "jcol jaw offset cannot be greater than the aperture half-size." << G4endl;
		  exit(1);
		}
	}
  if (std::abs(jaw1Offset) > (0.5*horizontalWidth - std::abs(jcolAperture)))
	{
	  G4cerr << __METHOD_NAME__ << "jcol \"" << name << "\" jaw1 offset is greater the element half width, jaw1 "
	         << "will not be constructed" << G4endl;
	  buildLeftJaw = false;
	}
  if (std::abs(jaw2Offset) > (0.5*horizontalWidth - std::abs(jcolAperture)))
	{
	  G4cerr << __METHOD_NAME__ << "jcol \"" << name << "\" jaw2 offset is greater the element half width, jaw2 "
			 << "will not be constructed" << G4endl;
	  buildRightJaw = false;
	}

  if (!buildLeftJaw || !buildRightJaw)
	{G4cerr << __METHOD_NAME__ << "warning no jaws being built" << G4endl; exit(1);}

  // gap orientation bool and generic aperture width instead of constantly checking for horizontal/vertical width
  jcolAperture = 0;
  apertureIsVertical = true;
  if (BDS::IsFinite(xAperture))
	{jcolAperture = xAperture;}
  else if (BDS::IsFinite(yAperture))
	{
 	  jcolAperture = yAperture;
	  apertureIsVertical = false;
	}

  // if the offset is the same as the aperture, the aperture is closed
  if ((jaw1Offset == jcolAperture) && (jaw2Offset == jcolAperture))
	{jcolAperture = 0;}
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
  if (sensitiveOuter)
    {RegisterSensitiveVolume(jawLV);}

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

void BDSCollimatorJaw::BuildJawCollimator()
{
  // build the vacuum volume only if the aperture is finite
	if (BDS::IsFinite(jcolAperture))
	  {
		// no inner solid being subtracted so nothing to do

		// vacuum solid still needed - should be either total height of the element but with width of aperture
		if (apertureIsVertical)
		  {
		    vacuumSolid = new G4Box(name + "_vacuum_solid",               // name
			 	   				    0.5 * jcolAperture - lengthSafety,    // x half width
									0.5 * horizontalWidth - lengthSafety, // y half width
									chordLength * 0.5);                   // z half length
		  }
		// or total width of the element but with height of aperture
		else
		  {
			vacuumSolid = new G4Box(name + "_vacuum_solid",               // name
			  					    0.5 * horizontalWidth - lengthSafety, // x half width
									0.5 * jcolAperture - lengthSafety,    // y half width
									chordLength * 0.5);                   // z half length
		  }
		RegisterSolid(vacuumSolid);
	  }

  // calculate position of vacuum boundaries
  G4double vacuumBoundary1 = jcolAperture;
  G4double vacuumBoundary2 = jcolAperture;

  if (BDS::IsFinite(jaw1Offset))
	{vacuumBoundary1 -= jaw1Offset;}
  if (BDS::IsFinite(jaw2Offset))
	{vacuumBoundary2 -= jaw2Offset;}

  // jaws have to fit inside containerLV so calculate jaw widths given offsets
  G4double jaw1Width = 0.5*horizontalWidth - std::abs(vacuumBoundary1);
  G4double jaw2Width = 0.5*horizontalWidth - std::abs(vacuumBoundary2);

  // centre of jaw and vacuum volumes for placements
  G4double jaw1Centre = 0.5*jaw1Width + vacuumBoundary1;
  G4double jaw2Centre = 0.5*jaw2Width + vacuumBoundary2;
  G4double vacuumCentre = 0.5*(vacuumBoundary1 - vacuumBoundary2);

  G4double jaw1Height = horizontalWidth;
  G4double jaw2Height = horizontalWidth;

  G4ThreeVector posOffset1;
  G4ThreeVector posOffset2;
  G4ThreeVector vacuumOffset;

  if (apertureIsVertical)
	{
	  posOffset1 = G4ThreeVector(jaw1Centre, 0, 0);
	  posOffset2 = G4ThreeVector(-jaw2Centre, 0, 0);
	  vacuumOffset = G4ThreeVector(vacuumCentre, 0, 0);
	}
  else
	{
 	  posOffset1 = G4ThreeVector(0, jaw1Centre, 0);
	  posOffset2 = G4ThreeVector(0, -jaw2Centre, 0);
	  vacuumOffset = G4ThreeVector(0, vacuumCentre, 0);
	  std::swap(jaw1Height,jaw1Width);
	  std::swap(jaw2Height,jaw2Width);
	}

  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  RegisterVisAttributes(collimatorVisAttr);

  // build jaws as appropriate
  if (buildLeftJaw)
	{
	  G4VSolid *jaw1Solid = new G4Box(name + "_jaw1_solid",
	  					  			  jaw1Width * 0.5 - lengthSafety,
						  			  jaw1Height * 0.5 - lengthSafety,
						  			  chordLength * 0.5 - lengthSafety);
	  RegisterSolid(jaw1Solid);

	  G4LogicalVolume* jaw1LV = new G4LogicalVolume(jaw1Solid,    		// solid
													collimatorMaterial, // material
												    name + "_jaw1_lv"); // name
	  jaw1LV->SetVisAttributes(collimatorVisAttr);

	  // user limits
	  jaw1LV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

	  // register with base class (BDSGeometryComponent)
	  RegisterLogicalVolume(jaw1LV);
	  RegisterSensitiveVolume(jaw1LV);

	  // place the jaw
	  G4PVPlacement* jaw1PV = new G4PVPlacement(nullptr,                 // rotation
	  										    posOffset1,              // position
											    jaw1LV,                  // its logical volume
											    name + "_jaw1_pv",       // its name
											    containerLogicalVolume,  // its mother volume
											    false,		             // no boolean operation
											    0,		                 // copy number
											    checkOverlaps);
	  RegisterPhysicalVolume(jaw1PV);
	}
  if (buildRightJaw)
	{
	  G4VSolid *jaw2Solid = new G4Box(name + "_jaw2_solid",
	    				   			  jaw2Width * 0.5 - lengthSafety,
									  jaw2Height * 0.5 - lengthSafety,
									  chordLength * 0.5 - lengthSafety);
	  RegisterSolid(jaw2Solid);

	  G4LogicalVolume* jaw2LV = new G4LogicalVolume(jaw2Solid,    		// solid
													collimatorMaterial, // material
													name + "_jaw2_lv"); // name

	  jaw2LV->SetVisAttributes(collimatorVisAttr);

	  // user limits
	  jaw2LV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

	  // register with base class (BDSGeometryComponent)
	  RegisterLogicalVolume(jaw2LV);
	  RegisterSensitiveVolume(jaw2LV);

	  // place the jaw
	  G4PVPlacement* jaw2PV = new G4PVPlacement(nullptr,                 // rotation
												posOffset2,              // position
												jaw2LV,                  // its logical volume
												name + "_jaw2_pv",       // its name
												containerLogicalVolume,  // its mother volume
												false,		             // no boolean operation
												0,		                 // copy number
												checkOverlaps);
	  RegisterPhysicalVolume(jaw2PV);
	}

  // place vacuum volume. No rotation as the volume is constructed with the correct dimensions.
  if (BDS::IsFinite(jcolAperture))
	{
	  G4LogicalVolume *vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
														vacuumMaterial,       // material
														name + "_vacuum_lv"); // name

	  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
	  vacuumLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
	  SetAcceleratorVacuumLogicalVolume(vacuumLV);
	  RegisterLogicalVolume(vacuumLV);

	  G4PVPlacement *vacPV = new G4PVPlacement(nullptr,                 // rotation
	  										   vacuumOffset,            // position
											   vacuumLV,                // its logical volume
											   name + "_vacuum_pv",     // its name
											   containerLogicalVolume,  // its mother  volume
											   false,                   // no boolean operation
											   0,                       // copy number
											   checkOverlaps);
	  RegisterPhysicalVolume(vacPV);
	}
}
