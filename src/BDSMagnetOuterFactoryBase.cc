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
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMaterials.hh"

#include "globals.hh" // geant4 types / globals

#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

G4double const BDSMagnetOuterFactoryBase::lengthSafetyLarge = 1*CLHEP::um;

BDSMagnetOuterFactoryBase::BDSMagnetOuterFactoryBase()
{
  lengthSafety       = BDSGlobalConstants::Instance()->LengthSafety();
  checkOverlaps      = BDSGlobalConstants::Instance()->CheckOverlaps();
  visDebug           = BDSGlobalConstants::Instance()->VisDebug();
  nSegmentsPerCircle = BDSGlobalConstants::Instance()->NSegmentsPerCircle();
  maxStepFactor      = 0.5;

  // initialise variables and pointers that'll be used by the factory
  CleanUp();
}

void BDSMagnetOuterFactoryBase::CleanUp()
{
  poleSolid            = nullptr;
  yokeSolid            = nullptr;
  containerSolid       = nullptr;
  magnetContainerSolid = nullptr;
  poleLV               = nullptr;
  yokeLV               = nullptr;
  containerLV          = nullptr;
  magnetContainerLV    = nullptr;
  yokePV               = nullptr;
  outerVisAttributes   = nullptr;

  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allSolids.clear();
  allVisAttributes.clear();

  magnetContainer = nullptr;

  inputFaceNormal  = G4ThreeVector(0,0,-1);
  outputFaceNormal = G4ThreeVector(0,0, 1);
}

void BDSMagnetOuterFactoryBase::CreateLogicalVolumes(G4String    name,
						     G4Colour*   colour,
						     G4Material* outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (poleSolid)
    {
      poleLV = new G4LogicalVolume(poleSolid,
				   outerMaterial,
				   name + "_pole_lv");
    }
  
  yokeLV = new G4LogicalVolume(yokeSolid,
			       outerMaterial,
			       name + "_yoke_lv");

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_outer_container_lv");

  magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
					  emptyMaterial,
					  name + "_container_lv");
  
  // VISUAL ATTRIBUTES
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*colour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(outerVisAttr);
  if (poleLV)
    {poleLV->SetVisAttributes(outerVisAttr);}
  yokeLV->SetVisAttributes(outerVisAttr);
  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
  magnetContainerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());

  // USER LIMITS
  auto outerUserLimits = BDSGlobalConstants::Instance()->GetDefaultUserLimits();
  if (poleLV)
    {poleLV->SetUserLimits(outerUserLimits);}
  yokeLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
  magnetContainerLV->SetUserLimits(outerUserLimits);
}

void BDSMagnetOuterFactoryBase::BuildMagnetContainerSolidAngled(G4String      name,
								G4double      magnetContainerLength,
								G4double      magnetContainerRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  magnetContainerRadius += lengthSafetyLarge; // extra margin
  magnetContainerSolid = new G4CutTubs(name + "_container_solid",   // name
				       0,                           // inner radius
				       magnetContainerRadius,       // outer radius
				       magnetContainerLength * 0.5, // z half length
				       0,                           // starting angle
				       CLHEP::twopi,                // sweep angle
				       inputFaceNormal,             // input face normal vector
				       outputFaceNormal);           // output fae normal vector

  magContExtent = BDSExtent(magnetContainerRadius, magnetContainerRadius, magnetContainerLength*0.5);
}


void BDSMagnetOuterFactoryBase::BuildMagnetContainerSolidStraight(G4String name,
								  G4double magnetContainerLength,
								  G4double magnetContainerRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  magnetContainerRadius += lengthSafetyLarge; // extra margin
  magnetContainerSolid = new G4Tubs(name + "_container_solid",   // name
				    0,                           // inner radius
				    magnetContainerRadius,       // outer radius
				    magnetContainerLength * 0.5, // z half length
				    0,                           // starting angle
				    CLHEP::twopi);               // sweep angle

  magContExtent = BDSExtent(magnetContainerRadius, magnetContainerRadius, magnetContainerLength*0.5);
}
  
void BDSMagnetOuterFactoryBase::CreateMagnetContainerComponent()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  magnetContainer = new BDSGeometryComponent(magnetContainerSolid,
					     magnetContainerLV,
					     magContExtent);
}


void BDSMagnetOuterFactoryBase::SetFaceNormals(BDSMagnetOuter* outer)
{
  outer->SetInputFaceNormal(inputFaceNormal);
  outer->SetOutputFaceNormal(outputFaceNormal);
}
  
