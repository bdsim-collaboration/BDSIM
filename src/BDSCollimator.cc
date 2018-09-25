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
#include "BDSCollimator.hh"

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

BDSCollimator::BDSCollimator(G4String  nameIn,
			     G4double  lengthIn,
			     G4double  horizontalWidthIn,
			     G4String  typeIn,
			     G4double  xApertureIn,
			     G4double  yApertureIn,
			     G4double  xOutApertureIn,
			     G4double  yOutApertureIn,
                 G4double  jaw1OffsetIn,
                 G4double  jaw2OffsetIn,
                 G4String  collimatorMaterialIn,
			     G4String  vacuumMaterialIn,
			     G4Colour* colourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, typeIn),
  horizontalWidth(horizontalWidthIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  xOutAperture(xOutApertureIn),
  yOutAperture(yOutApertureIn),
  jaw1Offset(jaw1OffsetIn),
  jaw2Offset(jaw2OffsetIn),
  collimatorMaterial(collimatorMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  colour(colourIn)
{
  if(horizontalWidth==0)
    {horizontalWidth = BDSGlobalConstants::Instance()->HorizontalWidth();}

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

  // build both jcol jaws by default
  buildJaw1 = true;
  buildJaw2 = true;

  // checks not applicable to jaw collimators
  if (type != "jcol")
    {
      if ((xAperture > 0.5 * horizontalWidth) || (yAperture > 0.5 * horizontalWidth))
        {
          G4cerr << __METHOD_NAME__ << "half aperture bigger than width!" << G4endl;
          G4cerr << "Horizontal width is " << horizontalWidth << " mm for component named: \""
                 << name << "\"" << G4endl;
          G4cerr << "x aperture " << xAperture << " mm, y aperture " << yAperture << " mm" << G4endl;
          exit(1);
        }

      if ((xOutAperture > 0.5 * horizontalWidth) || (yOutAperture > 0.5 * horizontalWidth))
        {
          G4cerr << __METHOD_NAME__ << "half aperture exit bigger than width!" << G4endl;
          G4cerr << "Horizontal width is " << horizontalWidth << " mm for component named: \""
                 << name << "\"" << G4endl;
          G4cerr << "x aperture " << xOutAperture << " mm, y aperture " << yOutAperture << " mm" << G4endl;
          exit(1);
        }

      if (BDS::IsFinite(xOutAperture) && (xAperture <= 0))
        {
          G4cout << __METHOD_NAME__
                 << "Warning - no entrance aperture set for collimator - exit aperture parameters will be ignored"
                 << G4endl;
        }

      if (BDS::IsFinite(xOutAperture) && BDS::IsFinite(yOutAperture) && BDS::IsFinite(xAperture) &&
          BDS::IsFinite(yAperture))
        {
          if ((xOutAperture / yOutAperture) != (xAperture / yAperture))
            {
              G4cout << __METHOD_NAME__ << "Warning - X/Y half axes ratio at entrance and exit apertures are not equal"
                     << G4endl;
            }
        }
    }
  if (type == "jcol")
    {
      if (BDS::IsFinite(xAperture) && BDS::IsFinite(yAperture))
        {
          G4cerr << __METHOD_NAME__
                 << "jcol cannot have both finite xsize and finite ysize, one aperture must be unspecified"
                 << G4endl;
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
          G4cout << __METHOD_NAME__ << "jcol \"" << name << "\" jaw1 offset is greater the element half width, jaw1 "
                 << "will not be constructed" << G4endl;
          buildJaw1 = false;
        }
      if (std::abs(jaw2Offset) > (0.5*horizontalWidth - std::abs(jcolAperture)))
        {
          G4cout << __METHOD_NAME__ << "jcol \"" << name << "\" jaw2 offset is greater the element half width, jaw2 "
                 << "will not be constructed" << G4endl;
          buildJaw1 = false;
        }
    }

  if (collimatorMaterialIn == "")
    {
      G4cout << __METHOD_NAME__ << "Warning - no material set for collimator - using copper" << G4endl;
      collimatorMaterial = "Copper";
    }

  collimatorSolid = nullptr;
  innerSolid      = nullptr;
  vacuumSolid     = nullptr;

  tapered = (BDS::IsFinite(xOutAperture) && BDS::IsFinite(yOutAperture));

  if (!colour)
    {colour = BDSColours::Instance()->GetColour("collimator");}
}

BDSCollimator::~BDSCollimator()
{;}

void BDSCollimator::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
			     horizontalWidth*0.5,
			     horizontalWidth*0.5,
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSCollimator::BuildVacuumVolume(G4RotationMatrix* vacuumRotation,
                                      G4ThreeVector vacuumPosition)
{
  G4Material *vMaterial = nullptr;

  if (vacuumMaterial == "")
    { vMaterial = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->vacuumMaterial; }
  else
    { vMaterial = BDSMaterials::Instance()->GetMaterial(vacuumMaterial); }

  G4LogicalVolume *vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                                  vMaterial,            // material
                                                  name + "_vacuum_lv"); // name

  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  vacuumLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
  SetAcceleratorVacuumLogicalVolume(vacuumLV);
  RegisterLogicalVolume(vacuumLV);

  G4PVPlacement *vacPV = new G4PVPlacement(vacuumRotation,          // rotation
                                           vacuumPosition,          // position
                                           vacuumLV,                // its logical volume
                                           name + "_vacuum_pv",     // its name
                                           containerLogicalVolume,  // its mother  volume
                                           false,                   // no boolean operation
                                           0,                       // copy number
                                           checkOverlaps);

  RegisterPhysicalVolume(vacPV);
}

void BDSCollimator::Build()
{
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  // separate build method for jaw collimator as it has multiple jaw solids and placements when open
  if (type == "jcol")
    {
      if (!BDS::IsFinite(jcolAperture))  // only one placement so revert back to original method
        {BuildCollimator();}
      else
        {BuildJawCollimator();}
    }
  else
    {BuildCollimator();}
}

void BDSCollimator::BuildCollimator()
{
  // Swap variables around if exit size is larger than entrance size
  // Rotation for tapered collimator (needed for tapered elliptical collimator)
  G4bool isOutLarger = ((xOutAperture > xAperture) && (yOutAperture > yAperture));
  G4RotationMatrix* colRotate;
  if (tapered && isOutLarger)
    {
      std::swap(xAperture,xOutAperture);
      std::swap(yAperture,yOutAperture);
      colRotate = new G4RotationMatrix;
      colRotate->rotateX(CLHEP::pi);
      RegisterRotationMatrix(colRotate);
    }
  else
    {colRotate = nullptr;}

  G4VSolid* outerSolid = new G4Box(name + "_outer_solid",
                                   horizontalWidth * 0.5 - lengthSafety,
                                   horizontalWidth * 0.5 - lengthSafety,
                                   chordLength * 0.5   - lengthSafety);
  RegisterSolid(outerSolid);

  G4bool buildVacuumAndAperture = (BDS::IsFinite(xAperture) && BDS::IsFinite(yAperture));

  // only do subtraction if aperture actually set
  if(buildVacuumAndAperture)
    {
      BuildInnerCollimator();

      collimatorSolid = new G4SubtractionSolid(name + "_collimator_solid", // name
                                               outerSolid,                 // solid 1
                                               innerSolid);                // minus solid 2
      RegisterSolid(collimatorSolid);
    }
  else
    {collimatorSolid = outerSolid;}

  G4Material* material = BDSMaterials::Instance()->GetMaterial(collimatorMaterial);
  G4LogicalVolume* collimatorLV = new G4LogicalVolume(collimatorSolid,          // solid
                                                      material,                 // material
                                                      name + "_collimator_lv"); // name

  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  collimatorLV->SetVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(collimatorVisAttr);

  // user limits
  collimatorLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(collimatorLV);
  RegisterSensitiveVolume(collimatorLV);

  G4PVPlacement* collPV = new G4PVPlacement(colRotate,               // rotation
                                            (G4ThreeVector)0,        // position
                                            collimatorLV,            // its logical volume
                                            name + "_collimator_pv", // its name
                                            containerLogicalVolume,  // its mother  volume
                                            false,		     // no boolean operation
                                            0,		             // copy number
                                            checkOverlaps);

  RegisterPhysicalVolume(collPV);

  if (buildVacuumAndAperture)
    {BuildVacuumVolume(colRotate, (G4ThreeVector) 0);}
}

void BDSCollimator::BuildJawCollimator()
{
  // build the vacuum volume only if the aperture is finite
  if (BDS::IsFinite(jcolAperture))
    {BuildInnerCollimator();}

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

  G4VSolid *jaw1Solid;
  G4VSolid *jaw2Solid;

  G4Material* material = BDSMaterials::Instance()->GetMaterial(collimatorMaterial);
  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  RegisterVisAttributes(collimatorVisAttr);

  // build jaws as appropriate
  if (buildJaw1)
    {
      jaw1Solid = new G4Box(name + "_jaw1_solid",
                            jaw1Width * 0.5 - lengthSafety,
                            jaw1Height * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety);
      RegisterSolid(jaw1Solid);

      G4LogicalVolume* jaw1LV = new G4LogicalVolume(jaw1Solid,    // solid
                                        material,                 // material
                                        name + "_jaw1_lv");       // name
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
  if (buildJaw2)
    {
      jaw2Solid = new G4Box(name + "_jaw2_solid",
                            jaw2Width * 0.5 - lengthSafety,
                            jaw2Height * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety);
      RegisterSolid(jaw2Solid);

      G4LogicalVolume* jaw2LV = new G4LogicalVolume(jaw2Solid,    // solid
                                        material,                 // material
                                        name + "_jaw2_lv");       // name

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
    {BuildVacuumVolume(nullptr, vacuumOffset);}

}

