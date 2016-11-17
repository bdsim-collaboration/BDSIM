#include "BDSBeamPipeFactoryCircularVacuum.hh"
#include "BDSBeamPipe.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh" // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>

class G4Material;

BDSBeamPipeFactoryCircularVacuum* BDSBeamPipeFactoryCircularVacuum::instance = nullptr;

BDSBeamPipeFactoryCircularVacuum* BDSBeamPipeFactoryCircularVacuum::Instance()
{
  if (instance == nullptr)
    {instance = new BDSBeamPipeFactoryCircularVacuum();}
  return instance;
}

BDSBeamPipeFactoryCircularVacuum::BDSBeamPipeFactoryCircularVacuum()
{;}

BDSBeamPipeFactoryCircularVacuum::~BDSBeamPipeFactoryCircularVacuum()
{
  instance = nullptr;
}

BDSBeamPipe* BDSBeamPipeFactoryCircularVacuum::CreateBeamPipe(G4String    nameIn,
							      G4double    lengthIn,
							      G4double    aper1In,
							      G4double    /*aper2In*/,
							      G4double    /*aper3In*/,
							      G4double    /*aper4In*/,
							      G4Material* vacuumMaterialIn,
							      G4double    /*beamPipeThicknessIn*/,
							      G4Material* /*beamPipeMaterialIn*/)
{
  // clean up after last usage
  CleanUp();
  
  containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
			      0,                            // inner radius
			      aper1In,                      // outer radius
			      lengthIn*0.5,                 // half length
			      0,                            // rotation start angle
			      CLHEP::twopi);                // rotation finish angle

  G4double containerRadius = aper1In + lengthSafetyLarge;
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryCircularVacuum::CreateBeamPipe(G4String      nameIn,
							      G4double      lengthIn,
							      G4ThreeVector inputFaceNormalIn,
							      G4ThreeVector outputFaceNormalIn,
							      G4double      aper1In,
							      G4double      /*aper2In*/,
							      G4double      /*aper3In*/,
							      G4double      /*aper4In */,
							      G4Material*   vacuumMaterialIn,
							      G4double      /*beamPipeThicknessIn*/,
							      G4Material*   /*beamPipeMaterialIn*/)
{
  // clean up after last usage
  CleanUp();
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
				 0,                            // inner radius
				 aper1In,                      // outer radius
				 lengthIn*0.5,                 // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputFaceNormal,              // input face normal
				 outputFaceNormal);            // rotation finish angle

  G4double containerRadius = aper1In + lengthSafetyLarge;
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, lengthIn, containerRadius);
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryCircularVacuum::CommonFinalConstruction(G4String    nameIn,
								       G4Material* vacuumMaterialIn,
								       G4double    lengthIn,
								       G4double    containerRadiusIn)
{
  // prepare a longer container subtraction solid
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4Tubs(nameIn + "_container_sub_solid",// name
					 0,                              // inner radius
					 containerRadiusIn,              // outer radius
					 lengthIn*4,                     // full length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle

  // We don't use BDSBeamPipeFactoryBase::CommonConstruction as this factory doesn't
  // produce a beam pipe solid etc - only vacuum so it wouldn't work.
  
  containerLV = new G4LogicalVolume(containerSolid,
				    vacuumMaterialIn,
				    nameIn + "_container_lv");

  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
  vacuumLV = containerLV; // copy pointer for referencing in BuildBeamPipeAndRegisterVolumes.

  // Don't force half step! Likely this will be used for thin volumes where one and only one
  // step is required.
  G4UserLimits* beamPipeUserLimits = new G4UserLimits("beampipe_cuts");
  beamPipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  allUserLimits.push_back(beamPipeUserLimits);

  // record extents
  BDSExtent ext = BDSExtent(containerRadiusIn, containerRadiusIn, lengthIn*0.5);
  
  return BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(ext, containerRadiusIn);
}
