#include "BDSBeamPipeFactoryBase.hh"

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"            // for calculateorientation

#include "globals.hh"                 // geant4 globals / types
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

void BDSBeamPipeFactoryBase::TestInputParameters(G4Material*&  vacuumMaterialIn,
						 G4double&     beamPipeThicknessIn,
						 G4Material*&  beamPipeMaterialIn)
{
  if (!vacuumMaterialIn)
    {vacuumMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());}

  if (beamPipeThicknessIn < 1e-10)
    {beamPipeThicknessIn = BDSGlobalConstants::Instance()->GetBeamPipeThickness();}

  if (!beamPipeMaterialIn)
    {beamPipeMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetBeamPipeMaterialName());}
}
  
void BDSBeamPipeFactoryBase::CommonConstruction(G4String    nameIn,
						G4Material* vacuumMaterialIn,
						G4Material* beamPipeMaterialIn,
						G4double    lengthIn)
{
  /// build logical volumes
  BuildLogicalVolumes(nameIn,vacuumMaterialIn,beamPipeMaterialIn);
  /// set visual attributes
  SetVisAttributes();
  /// set beampipe to sensitive detector (if specified)
  SetSensitivity();
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
  // build the logical volumes
  vacuumLV   = new G4LogicalVolume(vacuumSolid,
				   vacuumMaterialIn,
				   nameIn + "_vacuum_lv");
  
  beamPipeLV = new G4LogicalVolume(beamPipeSolid,
				   beamPipeMaterialIn,
				   nameIn + "_beampipe_lv");
  
  containerLV = new G4LogicalVolume(containerSolid,
				    vacuumMaterialIn,
				    nameIn + "_container_lv");
}

void BDSBeamPipeFactoryBase::SetVisAttributes() {
  // VISUAL ATTRIBUTES
  // set visual attributes
  // beampipe
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(G4Color(0.4,0.4,0.4));
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceSolid(true);
  beamPipeLV->SetVisAttributes(pipeVisAttr);
  // vacuum
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  // container
  if (BDSExecOptions::Instance()->GetVisDebug()) {
    containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
  } else {
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  }
}

void BDSBeamPipeFactoryBase::SetSensitivity() {
  // SENSITIVITY
  // make the beampipe sensitive if required (attachd Sensitive Detector Class)
  // all components are on axis and aligned to 0,0,0
  if (BDSGlobalConstants::Instance()->GetSensitiveBeamPipe())
    {
      //beampipes are sensitive - attach appropriate sd to the beampipe volume
      beamPipeLV->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());
    }
}

G4UserLimits* BDSBeamPipeFactoryBase::SetUserLimits(G4double lengthIn) {
  // USER LIMITS
  // set user limits based on bdsim user specified parameters
  G4UserLimits* beamPipeUserLimits = new G4UserLimits("beampipe_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  beamPipeUserLimits->SetMaxAllowedStep( lengthIn * maxStepFactor );
  beamPipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  beamPipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  //attach cuts to volumes
  vacuumLV->SetUserLimits(beamPipeUserLimits);
  beamPipeLV->SetUserLimits(beamPipeUserLimits);
  containerLV->SetUserLimits(beamPipeUserLimits);
  return beamPipeUserLimits;
}

void BDSBeamPipeFactoryBase::PlaceComponents(G4String nameIn) {
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    vacuumLV,                     // lv to be placed
		    nameIn + "_vacuum_pv",        // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    beamPipeLV,                   // lv to be placed
		    nameIn + "_beampipe_pv",      // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
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

  // REGISTER all lvs
  aPipe->RegisterLogicalVolume(vacuumLV); //using geometry component base class method
  aPipe->RegisterLogicalVolume(beamPipeLV);
  aPipe->RegisterLogicalVolume(containerLV);
  
  return aPipe;
}

void BDSBeamPipeFactoryBase::CalculateOrientations(G4double angleIn, G4double angleOut)
{
  orientationIn  = BDS::CalculateOrientation(angleIn);
  orientationOut = BDS::CalculateOrientation(angleOut);
}
