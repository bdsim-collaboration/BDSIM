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
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

BDSBeamPipeFactoryBase::BDSBeamPipeFactoryBase()
{
  lengthSafety              = BDSGlobalConstants::Instance()->LengthSafety();
  lengthSafetyLarge         = 1*CLHEP::um;
  checkOverlaps             = BDSGlobalConstants::Instance()->CheckOverlaps();
  maxStepFactor             = 0.5; // fraction of length for maximum step size
  nSegmentsPerCircle        = 50;
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
  allUserLimits.clear();
}


BDSBeamPipe* BDSBeamPipeFactoryBase::CreateBeamPipeAngledIn(G4String    nameIn,
							    G4double    lengthIn,
							    G4double    angleInIn, // the normal angle of the input face
							    G4double    aper1,
							    G4double    aper2,
							    G4double    aper3,
							    G4double    aper4,
							    G4Material* vacuumMaterialIn,
							    G4double    beamPipeThicknessIn,
							    G4Material* beamPipeMaterialIn)
{
  return CreateBeamPipeAngledInOut(nameIn,lengthIn,angleInIn,0,aper1,aper2,aper3,aper4,vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn);
}

BDSBeamPipe* BDSBeamPipeFactoryBase::CreateBeamPipeAngledOut(G4String    nameIn,
							     G4double    lengthIn,
							     G4double    angleOutIn, // the normal angle of the output face
							     G4double    aper1,
							     G4double    aper2,
							     G4double    aper3,
							     G4double    aper4,
							     G4Material* vacuumMaterialIn,
							     G4double    beamPipeThicknessIn,
							     G4Material* beamPipeMaterialIn)
{
  return CreateBeamPipeAngledInOut(nameIn,lengthIn,0,angleOutIn,aper1,aper2,aper3,aper4,vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn);
}
  
void BDSBeamPipeFactoryBase::CommonConstruction(G4String    nameIn,
						G4Material* vacuumMaterialIn,
						G4Material* beamPipeMaterialIn,
						G4double    lengthIn)
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
#ifndef NOUSERLIMITS
  /// set user limits
  SetUserLimits(lengthIn);
#endif
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
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // VISUAL ATTRIBUTES
  // set visual attributes
  // beampipe
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

G4UserLimits* BDSBeamPipeFactoryBase::SetUserLimits(G4double lengthIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // USER LIMITS
  // set user limits based on bdsim user specified parameters
  G4UserLimits* beamPipeUserLimits = new G4UserLimits("beampipe_cuts");
  beamPipeUserLimits->SetMaxAllowedStep( lengthIn * maxStepFactor );
  beamPipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  allUserLimits.push_back(beamPipeUserLimits);
  //attach cuts to volumes
  vacuumLV->SetUserLimits(beamPipeUserLimits);
  beamPipeLV->SetUserLimits(beamPipeUserLimits);
  containerLV->SetUserLimits(beamPipeUserLimits);
  return beamPipeUserLimits;
}

void BDSBeamPipeFactoryBase::PlaceComponents(G4String nameIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
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

BDSBeamPipe* BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(std::pair<double,double> extX,
								     std::pair<double,double> extY,
								     std::pair<double,double> extZ,
								     G4double containerRadius)
{  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extX,extY,extZ,
				       containerSubtractionSolid,
				       vacuumLV,false,containerRadius);

  // register objects
  aPipe->RegisterSolid(allSolids);
  aPipe->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  aPipe->RegisterPhysicalVolume(allPhysicalVolumes);
  aPipe->RegisterSensitiveVolume(beamPipeLV);
  aPipe->RegisterVisAttributes(allVisAttributes);
  aPipe->RegisterUserLimits(allUserLimits);
  
  return aPipe;
}
