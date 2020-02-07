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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSComponentFactory.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSExtentGlobal.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkDetectorConstruction.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSMaterials.hh"
#include "BDSSimpleComponent.hh"
#include "BDSParser.hh"
#include "BDSPhysicsUtilities.hh"

#include "parser/elementtype.h"

#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VisAttributes.hh"

#include <set>
#include <vector>

class BDSParticleDefinition;

BDSLinkDetectorConstruction::BDSLinkDetectorConstruction():
worldSolid(nullptr),
linkBeamline(nullptr)
{;}

BDSLinkDetectorConstruction::~BDSLinkDetectorConstruction(){;}

G4VPhysicalVolume* BDSLinkDetectorConstruction::Construct()
{
  BDSGlobalConstants*    globalConstants = BDSGlobalConstants::Instance();
  BDSParticleDefinition* designParticle  = nullptr; // set below.
  BDSParticleDefinition* beamParticle    = nullptr;
  G4bool beamDifferentFromDesignParticle = false;
  BDS::ConstructDesignAndBeamParticle(BDSParser::Instance()->GetBeam(),
				      globalConstants->FFact(),
				      designParticle,
				      beamParticle,
				      beamDifferentFromDesignParticle);

  auto componentFactory = new BDSComponentFactory(designParticle);
  auto beamline = BDSParser::Instance()->GetBeamline();

  std::vector<BDSLinkOpaqueBox*> opaqueBoxes = {};
  linkBeamline = new BDSBeamline();

  for (auto elementIt = beamline.begin(); elementIt != beamline.end(); ++elementIt)
    {
      GMAD::ElementType eType = elementIt->type;

      if (eType == GMAD::ElementType::_LINE || eType == GMAD::ElementType::_REV_LINE)
        {continue;}
      
      std::set<GMAD::ElementType> acceptedTypes = {GMAD::ElementType::_ECOL,
						   GMAD::ElementType::_RCOL,
						   GMAD::ElementType::_JCOL,
						   GMAD::ElementType::_CRYSTALCOL,
						   GMAD::ElementType::_ELEMENT};
      auto search = acceptedTypes.find(eType);
      if (search == acceptedTypes.end())
	{throw BDSException(G4String("Unsupported element type for link = " + GMAD::typestr(eType)));}

      // Only need first argument, the rest pertain to beamlines.
      BDSAcceleratorComponent* component = componentFactory->CreateComponent(&(*elementIt),
									     nullptr,
									     nullptr,
									     0);


      BDSLinkOpaqueBox* opaqueBox = new BDSLinkOpaqueBox(component, 0, 0/* XXX: index...  to do*/);

      opaqueBoxes.push_back(opaqueBox);

      BDSSimpleComponent* comp = new BDSSimpleComponent(opaqueBox->GetName(),
							opaqueBox,
							opaqueBox->GetExtent().DZ());
      
      linkBeamline->AddComponent(comp);

      // The placement transform refers to centre of the collimators,
      // so subtract half the collimator length (z) to get to the
      // opening of the collimator.
      auto it = linkBeamline->end();
      it--;
      G4Transform3D* placementTransform = (*it)->GetPlacementTransform();
      BDSExtent componentExtent = component->GetExtent();
      G4double componentHalfLength = componentExtent.DZ() / 2.0;
      auto entranceOffset = G4Translate3D(0.0, 0.0, -componentHalfLength);
      G4Transform3D openingTransform = entranceOffset * (*placementTransform);
      collimatorTransforms.push_back(openingTransform);
    }

  G4ThreeVector worldExtentAbs = linkBeamline->GetExtentGlobal().GetMaximumExtentAbsolute();
  worldExtentAbs *= 1.2;
  worldSolid = new G4Box("world_solid",
				worldExtentAbs.x(),
				worldExtentAbs.y(),
				worldExtentAbs.z());
  worldExtent = BDSExtent(worldExtentAbs);
  
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid,
				     BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
				     "world_lv");

  G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->ContainerVisAttr()));
  debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(debugWorldVis);
  worldLV->SetUserLimits(globalConstants->DefaultUserLimits());

  auto worldPV = new G4PVPlacement(nullptr,
				   G4ThreeVector(),
				   worldLV,
				   "world_pv",
				   nullptr,
				   false,
				   0,
				   true);

  for (auto element : *linkBeamline)
    {
      G4String placementName = element->GetPlacementName() + "_pv";
      G4Transform3D* placementTransform = element->GetPlacementTransform();
      G4int copyNumber = element->GetCopyNo();
      // auto pv = 
      new G4PVPlacement(*placementTransform,                  // placement transform
			placementName,                        // placement name
			element->GetContainerLogicalVolume(), // volume to be placed
			worldPV,                          // volume to place it in
			false,                                // no boolean operation
			copyNumber,                           // copy number
			true);                       // overlap checking
    }

  return worldPV;
}

void BDSLinkDetectorConstruction::AddLinkCollimator(const std::string& collimatorName,
                  const std::string& materialName,
                  G4double length,
                  G4double aperture,
                  G4double rotation,
                  G4double xOffset,
                  G4double yOffset)
{
  // build component
  // wrap in box
  // add to beam line
  // update world extents and world solid
  // place that one element

}




// for (auto const &element

// construct collimators using component factory

// Wrap in OpaqueBox instances, with index in line.

// Then place the OpaqueBoxes

// Record placement/transform with index.

//




