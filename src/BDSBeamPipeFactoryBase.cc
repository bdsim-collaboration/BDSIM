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
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSColours.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <limits>
#include <set>

BDSBeamPipeFactoryBase::BDSBeamPipeFactoryBase()
{
  BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  sensitiveBeamPipe     = g->SensitiveBeamPipe();
  sensitiveVacuum       = g->StoreELossVacuum();
  storeApertureImpacts  = g->StoreApertureImpacts();
  minKineticEnergy      = 0;
  CleanUpBase(); // non-virtual call in constructor
}

void BDSBeamPipeFactoryBase::CleanUp()
{
  CleanUpBase();
}

void BDSBeamPipeFactoryBase::CleanUpBase()
{
  FactoryBaseCleanUp();
  vacuumSolid               = nullptr;
  beamPipeSolid             = nullptr;
  containerSolid            = nullptr;
  containerSubtractionSolid = nullptr;
  vacuumLV                  = nullptr;
  beamPipeLV                = nullptr;
  containerLV               = nullptr;
  vacuumPV                  = nullptr;
  beamPipePV                = nullptr;

  inputFaceNormal  = G4ThreeVector(0,0,-1);
  outputFaceNormal = G4ThreeVector(0,0, 1);
}
  
void BDSBeamPipeFactoryBase::CommonConstruction(G4String    nameIn,
						G4Material* vacuumMaterialIn,
						G4Material* beamPipeMaterialIn,
						G4double    length)
{
  allSolids.insert(vacuumSolid);
  allSolids.insert(beamPipeSolid);
  /// build logical volumes
  BuildLogicalVolumes(nameIn,vacuumMaterialIn,beamPipeMaterialIn);
  /// set visual attributes
  SetVisAttributes();
  /// set user limits
  SetUserLimits(length);
  /// place volumes
  PlaceComponents(nameIn);
}

void BDSBeamPipeFactoryBase::BuildLogicalVolumes(G4String    nameIn,
						 G4Material* vacuumMaterialIn,
						 G4Material* beamPipeMaterialIn)
{
  // build the logical volumes
  vacuumLV   = new G4LogicalVolume(vacuumSolid,
				   vacuumMaterialIn,
				   nameIn + "_vacuum_lv");
  
  beamPipeLV = new G4LogicalVolume(beamPipeSolid,
				   beamPipeMaterialIn,
				   nameIn + "_beampipe_lv");

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    nameIn + "_container_lv");
  allLogicalVolumes.insert(vacuumLV);
  allLogicalVolumes.insert(beamPipeLV);
}

void BDSBeamPipeFactoryBase::SetVisAttributes()
{
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("beampipe"));
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(pipeVisAttr);
  beamPipeLV->SetVisAttributes(pipeVisAttr);
  // vacuum
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
}

void BDSBeamPipeFactoryBase::SetUserLimits(G4double length)
{
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length);

  if (BDSGlobalConstants::Instance()->BeamPipeIsInfiniteAbsorber())
    {minKineticEnergy = std::numeric_limits<double>::max();}

  // new beam pipe user limits, copy from updated default user limits above
  G4UserLimits* beamPipeUL = new G4UserLimits(*ul);
  if (BDS::IsFinite(minKineticEnergy))
    {beamPipeUL->SetUserMinEkine(minKineticEnergy);}

  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.insert(ul);}
  if (beamPipeUL != defaultUL) // if it's not the default register it
    {allUserLimits.insert(beamPipeUL);}
  vacuumLV->SetUserLimits(ul);
  beamPipeLV->SetUserLimits(beamPipeUL);
  containerLV->SetUserLimits(ul);
}

void BDSBeamPipeFactoryBase::PlaceComponents(G4String nameIn)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4  
  vacuumPV = new G4PVPlacement(nullptr,                  // no rotation
			       G4ThreeVector(),          // position
			       vacuumLV,                 // lv to be placed
			       nameIn + "_vacuum_pv",    // name
			       containerLV,              // mother lv to be placed in
			       false,                    // no boolean operation
			       0,                        // copy number
			       checkOverlaps);           // whether to check overlaps
  
  beamPipePV = new G4PVPlacement(nullptr,                      // no rotation
				 G4ThreeVector(),              // position
				 beamPipeLV,                   // lv to be placed
				 nameIn + "_beampipe_pipe_pv", // name
				 containerLV,                  // mother lv to be placed in
				 false,                        // no boolean operation
				 0,                            // copy number
				 checkOverlaps);               // whether to check overlaps
  allPhysicalVolumes.insert(vacuumPV);
  allPhysicalVolumes.insert(beamPipePV);
}

BDSBeamPipe* BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(BDSExtent extent,
								     G4double  containerRadius,
								     G4bool    containerIsCircular)
{  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extent,
				       containerSubtractionSolid,
				       vacuumLV,containerIsCircular,containerRadius,
				       inputFaceNormal, outputFaceNormal);

  // register objects
  aPipe->RegisterSolid(allSolids);
  aPipe->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  aPipe->RegisterPhysicalVolume(allPhysicalVolumes);
  if (sensitiveVacuum)
    {aPipe->RegisterSensitiveVolume(vacuumLV, BDSSDType::energydepvacuum);}
  if (beamPipeLV)
    {
      if (sensitiveBeamPipe && storeApertureImpacts)// in the case of the circular vacuum, there isn't a beampipeLV
	{aPipe->RegisterSensitiveVolume(beamPipeLV, BDSSDType::aperturecomplete);}
      else if (storeApertureImpacts)
	{aPipe->RegisterSensitiveVolume(beamPipeLV, BDSSDType::apertureimpacts);}
      else
	{aPipe->RegisterSensitiveVolume(beamPipeLV, BDSSDType::energydep);}
    }
  aPipe->RegisterUserLimits(allUserLimits);
  aPipe->RegisterVisAttributes(allVisAttributes);
  
  return aPipe;
}
