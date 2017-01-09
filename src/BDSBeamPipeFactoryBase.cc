#include "BDSBeamPipeFactoryBase.hh"

#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

BDSBeamPipeFactoryBase::BDSBeamPipeFactoryBase()
{
  lengthSafety        = BDSGlobalConstants::Instance()->LengthSafety();
  checkOverlaps       = BDSGlobalConstants::Instance()->CheckOverlaps();
  nSegmentsPerCircle  = BDSGlobalConstants::Instance()->NSegmentsPerCircle();
  CleanUp();
}

void BDSBeamPipeFactoryBase::CleanUp()
{
  vacuumSolid               = nullptr;
  beamPipeSolid             = nullptr;
  containerSolid            = nullptr;
  containerSubtractionSolid = nullptr;
  vacuumLV                  = nullptr;
  beamPipeLV                = nullptr;
  containerLV               = nullptr;
  vacuumPV                  = nullptr;
  beamPipePV                = nullptr;

  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allSolids.clear();
  allVisAttributes.clear();

  inputFaceNormal  = G4ThreeVector(0,0,-1);
  outputFaceNormal = G4ThreeVector(0,0, 1);
}
  
void BDSBeamPipeFactoryBase::CommonConstruction(G4String    nameIn,
						G4Material* vacuumMaterialIn,
						G4Material* beamPipeMaterialIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  allSolids.push_back(vacuumSolid);
  allSolids.push_back(beamPipeSolid);
  /// build logical volumes
  BuildLogicalVolumes(nameIn,vacuumMaterialIn,beamPipeMaterialIn);
  /// set visual attributes
  SetVisAttributes();
  /// set user limits
  SetUserLimits();
  /// place volumes
  PlaceComponents(nameIn);
}

void BDSBeamPipeFactoryBase::BuildLogicalVolumes(G4String    nameIn,
						 G4Material* vacuumMaterialIn,
						 G4Material* beamPipeMaterialIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
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
  allLogicalVolumes.push_back(vacuumLV);
  allLogicalVolumes.push_back(beamPipeLV);
}

void BDSBeamPipeFactoryBase::SetVisAttributes()
{
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("beampipe"));
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(pipeVisAttr);
  beamPipeLV->SetVisAttributes(pipeVisAttr);
  // vacuum
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
}

void BDSBeamPipeFactoryBase::SetUserLimits()
{
  auto beamPipeUserLimits = BDSGlobalConstants::Instance()->GetDefaultUserLimits();
  vacuumLV->SetUserLimits(beamPipeUserLimits);
  beamPipeLV->SetUserLimits(beamPipeUserLimits);
  containerLV->SetUserLimits(beamPipeUserLimits);
}

void BDSBeamPipeFactoryBase::PlaceComponents(G4String nameIn)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4  
  vacuumPV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
			       (G4ThreeVector)0,             // position
			       vacuumLV,                     // lv to be placed
			       nameIn + "_vacuum_pv",        // name
			       containerLV,                  // mother lv to be place in
			       false,                        // no boolean operation
			       0,                            // copy number
			       checkOverlaps);               // whether to check overlaps
  
  beamPipePV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
				 (G4ThreeVector)0,             // position
				 beamPipeLV,                   // lv to be placed
				 nameIn + "_beampipe_pv",      // name
				 containerLV,                  // mother lv to be place in
				 false,                        // no boolean operation
				 0,                            // copy number
				 checkOverlaps);               // whether to check overlaps
  allPhysicalVolumes.push_back(vacuumPV);
  allPhysicalVolumes.push_back(beamPipePV);
}

BDSBeamPipe* BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(BDSExtent extent,
								     G4double  containerRadius)
{  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extent,
				       containerSubtractionSolid,
				       vacuumLV,false,containerRadius,
				       inputFaceNormal, outputFaceNormal);

  // register objects
  aPipe->RegisterSolid(allSolids);
  aPipe->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  aPipe->RegisterPhysicalVolume(allPhysicalVolumes);
  if (beamPipeLV)// in the case of the circular vacuum, there isn't a beampipeLV
    {aPipe->RegisterSensitiveVolume(beamPipeLV);}
  aPipe->RegisterVisAttributes(allVisAttributes);
  
  return aPipe;
}
