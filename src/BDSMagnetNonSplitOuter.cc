/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBendBuilder.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSIntegratorSet.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetNonSplitOuter.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSSimpleComponent.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "parser/element.h"

#include <algorithm>
#include <cmath>

using namespace GMAD;

BDSMagnetNonSplitOuter::BDSMagnetNonSplitOuter(BDSMagnetType           typeIn,
					       BDSBeamPipeInfo*        beamPipeInfoIn,
					       BDSMagnetOuterInfo*     magnetOuterInfoIn,
					       BDSFieldInfo*           vacuumFieldInfoIn,
					       BDSFieldInfo*           outerFieldInfoIn,
					       G4bool                  isThinIn,
					       const Element*          elementIn,
					       BDSMagnetStrength*      stIn,
					       G4double                brhoIn,
					       const BDSIntegratorSet* integratorSetIn,
					       G4double                incomingFaceAngleIn,
					       G4double                outgoingFaceAngleIn,
					       G4bool                  buildFringeFieldsIn,
					       const GMAD::Element*    prevElementIn,
					       const GMAD::Element*    nextElementIn):
  BDSMagnet(typeIn, elementIn->name, elementIn->l*CLHEP::m, beamPipeInfoIn,
	    magnetOuterInfoIn, vacuumFieldInfoIn, -elementIn->angle, outerFieldInfoIn, isThinIn),
  element(elementIn),
  st(stIn),
  brho(brhoIn),
  integratorSet(integratorSetIn),
  incomingFaceAngle(incomingFaceAngleIn),
  outgoingFaceAngle(outgoingFaceAngleIn),
  buildFringeFields(buildFringeFieldsIn),
  prevElement(prevElementIn),
  nextElement(nextElementIn)
{
  beamPipeInfo    = beamPipeInfoIn;
  magnetOuterInfo = magnetOuterInfoIn;
  vacuumFieldInfo = vacuumFieldInfoIn;
  outerFieldInfo  = outerFieldInfoIn;
  namedVacuumVolumes = BDS::SplitOnWhiteSpace(G4String(element->namedVacuumVolumes));

  // Calculate number of sbends to split parent into
  G4int nSBends = BDS::CalculateNSBendSegments(arcLength, angle, incomingFaceAngle,
                                                 outgoingFaceAngle);

  curvilinearSplitNumber = nSBends;
}


void BDSMagnetNonSplitOuter::SBendWithSingleOuter(const G4String& elementName)
{
  BDSAcceleratorComponent* pipeLine;
  
  // Creation of the beampipe by calling the BDS::BuildSBendLine with element magnetGeometryType to "none"
  if (element->apertureType != "none")
    {
      Element el = Element(*element);
      el.magnetGeometryType = "none";
      el.l -= lengthSafety;
      pipeLine = BDS::BuildSBendLine(elementName,&el,st,brho,integratorSet,incomingFaceAngle,
				     outgoingFaceAngle,buildFringeFields,prevElement,nextElement);
      pipeLine->Initialise();
    }
  
  // Create of a beampipe for BDSMagnetOuterFactory::Instance()->CreateMagnetOuter() with the correct geometry information
  BDSBeamPipeInfo* beamPipeInfoTmp = new BDSBeamPipeInfo(*beamPipeInfo);
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(element->angle/2 + incomingFaceAngle,
								     element->angle/2 + outgoingFaceAngle);
  beamPipeInfoTmp->inputFaceNormal = faces.first;
  beamPipeInfoTmp->outputFaceNormal = faces.second;
  
  BDSBeamPipe* beamPipeTmp = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name+"_bp",
                                                                            arcLength - 2*lengthSafety,
									    beamPipeInfoTmp);
  
  // Create the Magnet outer
  G4double outerLength = chordLength - 2*lengthSafety;
  outer = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(BDSMagnetType::sectorbend,
							       magnetOuterInfo,
							       outerFieldInfo,
							       outerLength,
							       chordLength,
							       beamPipeTmp);
  
  delete beamPipeTmp;
  delete beamPipeInfoTmp;
  
  BDSGeometryComponent* container = outer->GetMagnetContainer();
  containerLogicalVolume = container->GetContainerLogicalVolume();
  containerSolid    = container->GetContainerSolid()->Clone();
  G4ThreeVector contOffset = container->GetPlacementOffset();

  BDSGeometryExternal* outerGDML = outer->ExternalGeometry();
  std::set<G4LogicalVolume*> vacuumVols;

  if (outerGDML) // the dynamic cast will only work if it's loaded as GDML//
      {
        vacuumVols = outerGDML->VacuumVolumes();
        for (auto vol : vacuumVols)
	  {
            G4String name = vol->GetName();
            const std::set<G4VPhysicalVolume*> &physicalVols = outerGDML->GetAllPhysicalVolumes();
            for (auto volp : physicalVols)
	      {
                if (name == volp->GetLogicalVolume()->GetName())
		  {
                    G4RotationMatrix *mt = volp->GetRotation();
                    if (!mt)
		      {mt = new G4RotationMatrix();}
                    CLHEP::Hep3Vector v = volp->GetTranslation();
                    G4Transform3D* transform = new G4Transform3D(*mt, v);
                    outerFieldInfo->SetTransform(*transform);
                }
            }
        }

        // determine key for this specific magnet instance
        G4String scalingKey = DetermineScalingKey(magnetType);

        BDSMagnetStrength* scalingStrength = vacuumFieldInfo ? vacuumFieldInfo->MagnetStrength() : nullptr;
        // Attach to the container but don't propagate to daughter volumes. This ensures
        // any gap between the beam pipe and the outer also has a field.
        BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
                                                                  containerLogicalVolume,
                                                                  false,
                                                                  scalingStrength,
                                                                  scalingKey);

        BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                                  vacuumVols,
                                                                  true);

      }

  
  // set the main offset of the whole magnet which is placed w.r.t. the
  // zero coordinate of the container solid
  SetPlacementOffset(contOffset);
  RegisterDaughter(outer);
  InheritExtents(container, contOffset); // update extents
  // Only clear after extents etc have been used
  outer->ClearMagnetContainer();
  endPieceBefore = outer->EndPieceBefore();
  endPieceAfter  = outer->EndPieceAfter();
  /// Update record of normal vectors now beam pipe has been constructed.
  SetInputFaceNormal(BDS::RotateToReferenceFrame(outer->InputFaceNormal(), angle));
  SetOutputFaceNormal(BDS::RotateToReferenceFrame(outer->OutputFaceNormal(), -angle));

  // check if the beampipe and outer geometry elements must be placed in the container logical volume
  if (!element->extractOuterContainer) 
    {
      // check if the beampipe elements must be placed in the container logical volume
      if (element->apertureType != "none") 
        {
          // Create a beamline receiving the beampipe elements with the correct offset and angle
          G4double offsetAngle = element->angle/2;
          G4double offsetLength = chordLength/2;
  
          G4ThreeVector initialGlobalPosition = G4ThreeVector(0,0, -offsetLength);
          G4ThreeVector u = G4ThreeVector( std::cos(-offsetAngle), 0, std::sin(-offsetAngle));
          G4ThreeVector v = G4ThreeVector(0, 1,0);
          G4ThreeVector w = G4ThreeVector(-std::sin(-offsetAngle), 0, std::cos(-offsetAngle));
          G4RotationMatrix* initialGlobalRotation = new G4RotationMatrix(u, v, w);
          G4double          initialS = 0;
	  BDSBeamline* beamline = new BDSBeamline(initialGlobalPosition, initialGlobalRotation, initialS);
	  beamline->AddComponent(pipeLine);
	  G4int i = 0;
	  for (auto el : *beamline)
            {
	      G4String placementName = el->GetPlacementName() + "_pv";
	      G4Transform3D* placementTransform = el->GetPlacementTransform();
	      G4int copyNumber = i;
	      auto vv = new G4PVPlacement(*placementTransform,             // placement transform
					  el->GetAcceleratorComponent()->GetContainerLogicalVolume(), // volume to be placed
					  placementName,                   // placement name
					  containerLogicalVolume,          // volume to place it in
					  false,                           // no boolean operation
					  copyNumber,                      // copy number
					  false);                          // overlap checking
	      
	      i++; // for incremental copy numbers
	      
	      if (vv->CheckOverlaps() && checkOverlaps)
                {throw BDSException(__METHOD_NAME__, "Overlapping detected for the beampipe elements");}
	      RegisterPhysicalVolume(vv);    
            }
	  delete beamline;
        }
      
      G4ThreeVector outerOffset = outer->GetPlacementOffset();
      
      auto gdml_world = outer->GetContainerLogicalVolume();

      // Check if the outer logical volume (GDML world volume) must be placed into the container logical volume
      if (element->includeGdmlWorldVolume)
        {
	  G4Transform3D placementTransform;
	  auto vv = new G4PVPlacement(placementTransform,            // placement transform
				      gdml_world,                    // volume to be placed
				      gdml_world->GetName() + "_pv", // placement name
				      containerLogicalVolume,        // volume to place it in
				      false,                         // no boolean operation
				      0,                             // copy number
				      false);                        // overlap checking
	  
	  if (vv->CheckOverlaps() && checkOverlaps)
            {throw BDSException(__METHOD_NAME__, "Overlapping detected for the outer elements");}
	  RegisterPhysicalVolume(vv); 
        }
      
      for (G4int j = 0; j < (G4int)gdml_world->GetNoDaughters(); j++)
        {
	  const auto& pv = gdml_world->GetDaughter(j);
	  G4String placementName = pv->GetName() + "_pv";
	  G4int copyNumber = 1;
	  
	  if (!element->includeGdmlWorldVolume)
            {
	      // if the GDML world volume has not been placed in the container logical volume,
	      // placed the outer geometry elements one by one instead
	      auto vv = new G4PVPlacement(pv->GetRotation(), pv->GetTranslation(), // placement transform
					  pv->GetLogicalVolume(),                  // volume to be placed
					  placementName,                           // placement name
					  containerLogicalVolume,                  // volume to place it in
					  false,                                   // no boolean operation
					  copyNumber,                              // copy number
					  false);                                  // overlap checking
	      
	      if (vv->CheckOverlaps() && checkOverlaps)
                {throw BDSException(__METHOD_NAME__, "Overlapping detected for the outer elements");}
	      RegisterPhysicalVolume(vv);
            }
        }
    }
}


void BDSMagnetNonSplitOuter::Build()
{
  SBendWithSingleOuter(element->name);
}
