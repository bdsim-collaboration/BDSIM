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
#include "BDSColours.hh"
#include "BDSCrystal.hh"
#include "BDSCrystalFactory.hh"
#include "BDSCrystalInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

// only use the crystal extensions if using 10.4.p00 upwards
#if G4VERSION_NUMBER > 1039
#include "G4ExtendedMaterial.hh"
#include "G4CrystalExtension.hh"
#include "G4CrystalUnitCell.hh"
#include "G4ChannelingMaterialData.hh"
#include "G4LogicalCrystalVolume.hh"
#endif

BDSCrystalFactory::BDSCrystalFactory():
  maxStepFactor(1.1),
  nSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle())
{
  CleanUp();
}

void BDSCrystalFactory::CleanUp()
{
  crystalSolid      = nullptr;
  crystalLV         = nullptr;
  placementOffset   = G4ThreeVector();
  placementRotation = nullptr;
  
  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allSolids.clear();
  allUserLimits.clear();
  allVisAttributes.clear();
}

BDSCrystal* BDSCrystalFactory::CreateCrystal(const G4String& name,
					     const BDSCrystalInfo* recipe)
{
  CleanUp();
  
  switch(recipe->shape.underlying())
    {
    case BDSCrystalType::box:
      {return CreateCrystalBox(name, recipe); break;}
    case BDSCrystalType::cylinder:
      {return CreateCrystalCylinder(name, recipe); break;}
    case BDSCrystalType::torus:
      {return CreateCrystalTorus(name, recipe); break;}
    default:
      {return nullptr; break;}
    }
}
  
void BDSCrystalFactory::CommonConstruction(const G4String&       nameIn,
					   const BDSCrystalInfo* recipe)
{
  allSolids.push_back(crystalSolid);

  // only in g4.10.4 onwards do we build the crystal extensions - otherwise regular LV
#if G4VERSION_NUMBER > 1039
  G4ExtendedMaterial* crystalMat = new G4ExtendedMaterial("crystal.material", recipe->material);
  
  crystalMat->RegisterExtension(std::unique_ptr<G4CrystalExtension>(new G4CrystalExtension(crystalMat)));
  G4CrystalExtension* crystalExtension = dynamic_cast<G4CrystalExtension*>(crystalMat->RetrieveExtension("crystal"));

  G4CrystalUnitCell* uCell = new G4CrystalUnitCell(recipe->sizeA,
						   recipe->sizeB,
						   recipe->sizeC,
						   recipe->alpha,
						   recipe->beta,
						   recipe->gamma,
						   recipe->spaceGroup);
  crystalExtension->SetUnitCell(uCell);

  crystalMat->RegisterExtension(std::unique_ptr<G4ChannelingMaterialData>(new G4ChannelingMaterialData("channeling")));

  G4ChannelingMaterialData* crystalChannelingData = (G4ChannelingMaterialData*)crystalMat->RetrieveExtension("channeling");
  G4String fileName = BDS::GetFullPath(recipe->data);
  if (!BDS::FileExists(fileName + "_pot.txt"))
    {
      G4cout << __METHOD_NAME__ << "No such crystal data files beginnging with: " << G4endl
	     << "\"" << fileName << "\"" << G4endl;
      exit(1);
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Raw data path: " << recipe->data << G4endl;
  G4cout << __METHOD_NAME__ << "Using crystal data: " << fileName << G4endl;
#endif
  crystalChannelingData->SetFilename(fileName);
  
  //G4ThreeVector bendingAngles = G4ThreeVector(recipe->bendingAngleYAxis);//, // bend induced in X
  //recipe->bendingAngleZAxis,
  //0);
  G4double bendingRadius = BendingRadiusHorizontal(recipe);
  
  crystalChannelingData->SetBR(bendingRadius);
  
  crystalLV = new G4LogicalCrystalVolume(crystalSolid,
					 crystalMat,
					 nameIn + "_crystal_lv");
#else
  // build logical volumes
  crystalLV = new G4LogicalVolume(crystalSolid,
				  crystalMaterialIn,
				  nameIn + "_crystal_lv");
#endif
  allLogicalVolumes.push_back(crystalLV);
  
  SetVisAttributes();
  SetUserLimits(recipe->lengthZ);
}

void BDSCrystalFactory::SetVisAttributes()
{
  G4VisAttributes* crysVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("crystal"));
  crysVisAttr->SetVisibility(true);
  crysVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(crysVisAttr);
  crystalLV->SetVisAttributes(crysVisAttr);
}

void BDSCrystalFactory::SetUserLimits(const G4double& length)
{
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length*maxStepFactor);

  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.push_back(ul);}
  crystalLV->SetUserLimits(ul);
}

BDSCrystal* BDSCrystalFactory::BuildCrystalObject(const BDSExtent& extent)
{  
  // build the BDSCrystal instance and return it
  BDSCrystal* aCrystal = new BDSCrystal(crystalSolid, crystalLV,
					extent, placementOffset, placementRotation);

  // register objects
  aCrystal->RegisterSensitiveVolume(crystalLV);
  aCrystal->RegisterUserLimits(allUserLimits);
  aCrystal->RegisterVisAttributes(allVisAttributes);
  
  return aCrystal;
}

BDSCrystal* BDSCrystalFactory::CreateCrystalBox(const G4String&       nameIn,
						const BDSCrystalInfo* recipe)
{
  crystalSolid = new G4Box(nameIn + "_solid",
			   recipe->lengthX * 0.5,
			   recipe->lengthY * 0.5,
			   recipe->lengthZ * 0.5);

  CommonConstruction(nameIn, recipe);

  BDSExtent ext = BDSExtent(recipe->lengthX * 0.5,
			    recipe->lengthY * 0.5,
			    recipe->lengthZ * 0.5);
  
  return BuildCrystalObject(ext); // no placement offset - leave as default
}

BDSCrystal* BDSCrystalFactory::CreateCrystalCylinder(const G4String&       nameIn,
						     const BDSCrystalInfo* recipe)
{
  G4double ba  = recipe->bendingAngleYAxis; // bending angle

  // if no bending angle, create a box as that's all we can create
  if (!BDS::IsFinite(ba))
  {return CreateCrystalBox(nameIn, recipe);}

  G4double xBR = std::abs(BendingRadiusHorizontal(recipe));
  G4double thickness = recipe->lengthX;
  
  // calculate start angle and sweep angle
  G4double startAngle;
  G4double sweepAngle;
  if (ba >= 0)
    {
      startAngle = CLHEP::twopi - 0.5 * ba;
      sweepAngle = ba;
    }
  else
    {
      startAngle = CLHEP::pi - 0.5*std::abs(ba);
      sweepAngle = std::abs(ba);
    }
  
  crystalSolid = new G4Tubs(nameIn + "_solid",
			    xBR - 0.5*thickness,
			    xBR + 0.5*thickness,
			    (recipe->lengthZ)*0.5,
			    startAngle,
			    sweepAngle);

  CommonConstruction(nameIn, recipe);

  G4double xLow;
  G4double xHi;
  if (!BDS::IsFinite(ba))
    {
      xLow = -(thickness*0.5);
      xHi  = -xLow;
    }
  else if (ba <= 0)
    {
      xLow = recipe->lengthX * 0.5;
      xHi  = recipe->lengthX * 0.5;
    }
  else
    {
      xLow = recipe->lengthX * 0.5;
      xHi  = recipe->lengthX * 0.5;
    }

  placementOffset = G4ThreeVector(-BendingRadiusHorizontal(recipe), 0, 0);
  placementRotation = new G4RotationMatrix();
  placementRotation->rotateX(-CLHEP::halfpi);
  
  BDSExtent ext = BDSExtent(xLow, xHi,
			    recipe->lengthY * 0.5, recipe->lengthY * 0.5,
			    recipe->lengthZ * 0.5, recipe->lengthZ * 0.5);
  
  return BuildCrystalObject(ext);
}

BDSCrystal* BDSCrystalFactory::CreateCrystalTorus(const G4String&       nameIn,
						  const BDSCrystalInfo* recipe)
{
  G4double xBR = BendingRadiusHorizontal(recipe);
  G4double zBR = BendingRadiusVertical(recipe);
   return nullptr;
}
