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
			     G4String  collimatorMaterialIn,
			     G4String  vacuumMaterialIn,
			     G4Colour* colourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, typeIn),
  horizontalWidth(horizontalWidthIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  xOutAperture(xOutApertureIn),
  yOutAperture(yOutApertureIn),
  collimatorMaterial(collimatorMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  colour(colourIn)
{
  if(horizontalWidth==0)
    {horizontalWidth = BDSGlobalConstants::Instance()->HorizontalWidth();}

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

void BDSCollimator::Build()
{
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  // seperate build method for now as the jaw collimator has multiple jaw solids and placements
  if (type == "jcol")
    {BuildJawCollimator();}
  else
    {
      // now build the collimator
      G4VSolid* outerSolid = new G4Box(name + "_outer_solid",
                       horizontalWidth * 0.5 - lengthSafety,
                       horizontalWidth * 0.5 - lengthSafety,
                       chordLength * 0.5   - lengthSafety);
      RegisterSolid(outerSolid);

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
        {
          G4Material* vMaterial = nullptr;
          if (vacuumMaterial == "")
        {vMaterial = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->vacuumMaterial;}
          else
        {vMaterial = BDSMaterials::Instance()->GetMaterial(vacuumMaterial);}
          G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                  vMaterial,            // material
                                  name + "_vacuum_lv"); // name

          vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
          vacuumLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
          SetAcceleratorVacuumLogicalVolume(vacuumLV);
          RegisterLogicalVolume(vacuumLV);

          G4PVPlacement* vacPV = new G4PVPlacement(colRotate,               // rotation
                               (G4ThreeVector)0,        // position
                               vacuumLV,                // its logical volume
                               name + "_vacuum_pv",     // its name
                               containerLogicalVolume,  // its mother  volume
                               false,		        // no boolean operation
                               0,		        // copy number
                               checkOverlaps);

          RegisterPhysicalVolume(vacPV);
        }
    }
}

void BDSCollimator::BuildJawCollimator()
{
  G4double jcolAperture = 0;
  G4double apertureIsVertical = true;
  if (BDS::IsFinite(xAperture))
    {jcolAperture = xAperture;}
  else if (BDS::IsFinite(yAperture))
    {
      jcolAperture = yAperture;
      apertureIsVertical = false;
    }
  else
    {jcolAperture = 0;}

  // now build the collimator
  BuildInnerCollimator();

  G4double jawWidth = 0.5 * (horizontalWidth - 2*jcolAperture);
  G4VSolid *jaw1Solid;
  G4VSolid *jaw2Solid;

  if (apertureIsVertical)
    {
      jaw1Solid = new G4Box(name + "_jaw1_solid",
                            jawWidth * 0.5 - lengthSafety,
                            horizontalWidth * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety);

      jaw2Solid = new G4Box(name + "_jaw2_solid",
                            jawWidth * 0.5 - lengthSafety,
                            horizontalWidth * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety);
    }
  else
    {
      jaw1Solid = new G4Box(name + "_jaw1_solid",
                            horizontalWidth * 0.5 - lengthSafety,
                            jawWidth * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety);

      jaw2Solid = new G4Box(name + "_jaw2_solid",
                            horizontalWidth * 0.5 - lengthSafety,
                            jawWidth * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety);
    }


  RegisterSolid(jaw1Solid);
  RegisterSolid(jaw2Solid);

  G4Material* material = BDSMaterials::Instance()->GetMaterial(collimatorMaterial);
  G4LogicalVolume* jaw1LV = new G4LogicalVolume(jaw1Solid,    // solid
                                    material,                 // material
                                    name + "_jaw1_lv");       // name

  G4LogicalVolume* jaw2LV = new G4LogicalVolume(jaw2Solid,    // solid
                                    material,                 // material
                                    name + "_jaw2_lv");       // name


  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  jaw1LV->SetVisAttributes(collimatorVisAttr);
  jaw2LV->SetVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(collimatorVisAttr);

  // user limits
  jaw1LV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
  jaw2LV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(jaw1LV);
  RegisterSensitiveVolume(jaw1LV);
  RegisterLogicalVolume(jaw2LV);
  RegisterSensitiveVolume(jaw2LV);

  G4ThreeVector offset1;
  G4ThreeVector offset2;

  if (apertureIsVertical)
    {
      offset1 = G4ThreeVector(-0.5 * (jcolAperture + jawWidth), 0, 0);
      offset2 = G4ThreeVector(0.5 * (jcolAperture + jawWidth), 0, 0);
    }
  else
    {
      offset1 = G4ThreeVector(0, -0.5 * (jcolAperture + jawWidth), 0);
      offset2 = G4ThreeVector(0, 0.5 * (jcolAperture + jawWidth), 0);
    }

  // place the two jaws
  G4PVPlacement* jaw1PV = new G4PVPlacement(nullptr,                 // rotation
                                            offset1,                 // position
                                            jaw1LV,                  // its logical volume
                                            name + "_jaw1_pv",       // its name
                                            containerLogicalVolume,  // its mother volume
                                            false,		             // no boolean operation
                                            0,		                 // copy number
                                            checkOverlaps);
  RegisterPhysicalVolume(jaw1PV);

  G4PVPlacement* jaw2PV = new G4PVPlacement(nullptr,                 // rotation
                                            offset2,                 // position
                                            jaw2LV,                  // its logical volume
                                            name + "_jaw2_pv",       // its name
                                            containerLogicalVolume,  // its mother volume
                                            false,		             // no boolean operation
                                            0,		                 // copy number
                                            checkOverlaps);
  RegisterPhysicalVolume(jaw2PV);

  if (BDS::IsFinite(xAperture))
    {
      G4Material *vMaterial = nullptr;
      if (vacuumMaterial == "")
        {vMaterial = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->vacuumMaterial; }
      else
        {vMaterial = BDSMaterials::Instance()->GetMaterial(vacuumMaterial); }
      G4LogicalVolume *vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                                      vMaterial,            // material
                                                      name + "_vacuum_lv"); // name

      vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      vacuumLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
      SetAcceleratorVacuumLogicalVolume(vacuumLV);
      RegisterLogicalVolume(vacuumLV);

      // rotate the vacuum volume if the aperture is horizontal
      G4RotationMatrix* vacuumRotation = nullptr;
      if (!apertureIsVertical)
        {
          vacuumRotation = new G4RotationMatrix;
          vacuumRotation->rotateZ(0.5*CLHEP::pi);
        }

      G4PVPlacement *vacPV = new G4PVPlacement(nullptr,          // rotation
                                               (G4ThreeVector) 0,       // position
                                               vacuumLV,                // its logical volume
                                               name + "_vacuum_pv",     // its name
                                               containerLogicalVolume,  // its mother  volume
                                               false,                   // no boolean operation
                                               0,                       // copy number
                                               checkOverlaps);

      RegisterPhysicalVolume(vacPV);
    }

}

