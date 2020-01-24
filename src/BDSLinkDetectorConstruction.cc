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

#include "G4types.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkDetectorConstruction.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSPhysicsUtilities.hh"

// class BDSParticleDefinition

BDSLinkDetectorConstruction::BDSLinkDetectorConstruction(){;}

BDSLinkDetectorConstruction::~BDSLinkDetectorConstruction(){;}

G4VPhysicalVolume* BDSLinkDetectorConstruction::Construct()
  {

    auto globalConstants = BDSGlobalConstants::Instance();
    BDSParticleDefinition* designParticle = nullptr; // set below.
    G4bool beamDifferentFromDesignParticle = false;
    BDS::ConstructDesignAndBeamParticle(BDSParser::Instance()->GetBeam(),
					globalConstants->FFact(),
					designParticle,
					nullptr,
					false);

    auto componentFactory = new BDSComponentFactory(designParticle);
    auto beamline = BDSParser::Instance()->GetBeamline();

    G4double gapBetweenOpaqueBoxes = 10*CLHEP::cm;
    G4double totalBoxesLength = 0.0;
    G4int nboxes = 0

    std::vector<BDSLinkOpaqueBox> opaqueBoxes = {};

    BDSBeamline* bl = new BDSBeamline();

    for (auto elementIt = beamline.begin(); elementIt != beamline.end(); ++elementIt)
      {
	auto type = elementIt->type;
        if (type != GMAD::ElementType::_ECOL ||
            type != GMAD::ElementType::_RCOL ||
            type != GMAD::ElementType::_JCOL ||
            type != GMAD::ElementType::_CRYSTALCOL)
	  {
	    throw BDSException(G4String("Unsupported element type for link = " +
					GMAD::typestr(type)));
	  }

          // Only need first argument, the rest pertain to beamlines.
          BDSAcceleratorComponent *component =
              theComponentFactory->CreateComponent(&(*elementIt), nullptr,
                                                   nullptr, 0.0);


	  auto opaqueBox = BDSLinkOpaqueBox(component);
	  totalCollimatorLength += opaqueBox.GetExtent().DZ();

	  opaqueBoxes.push_back(opaqueBox);

	  G4double length = geom->GetExtent().DZ();
	  BDSSimpleComponent* comp = new BDSSimpleComponent(placement.name + "_" + geom->GetName(),
							    geom,
							    length);

	  bl->AddComponent(comp);

      }


    BDSExtent worldExtent = blm->GetExtent();
    auto worldSolid = new G4Box(worldName + "_solid",
				worldExtent.DX() * 1.2,
				worldExtent.DY() * 1.2,
				worldExtent.DZ() * 1.2);
    auto worldLV = new G4LogicalVolume(worldSolid,
				       BDSMaterials::GetMaterial("G4_Galactic"),
				       "world_lv");

    auto debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->ContainerVisAttr()));
    debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
    worldLV->SetVisAttributes(debugWorldVis);
    worldLV->SetUserLimits(globalConstants->DefaultUserLimits());

    auto worldPV = new G4PVPlacement(nullptr,
				     G4ThreeVector(),
				     "world_pv",
				     nullptr,
				     false,
				     0,
				     true);
				       


    for (auto element : *beamline)
      {

	G4String placementName = element->GetPlacementName() + "_pv";
	G4Transform3D* placementTransform = element->GetPlacementTransform();
	G4int copyNumber = element->GetCopyNumber();
	auto pv = new G4PVPlacement(*placementTransform,                  // placement transform
				    placementName,                        // placement name
				    element->GetContainerLogicalVolume(), // volume to be placed
				    worldPV,                          // volume to place it in
				    false,                                // no boolean operation
				    copyNumber,                           // copy number
				    true);                       // overlap checking
      }



  }




    // for (auto const &element

    // construct collimators using component factory

    // Wrap in OpaqueBox instances, with index in line.

    // Then place the OpaqueBoxes

    // Record placement/transform with index.

    //

    return nullptr;

  }
