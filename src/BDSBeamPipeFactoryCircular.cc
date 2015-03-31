#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipe.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>                         // for std::pair


BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::_instance = 0;

BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactoryCircular();}
  return _instance;
}

BDSBeamPipeFactoryCircular::BDSBeamPipeFactoryCircular()
{
  lengthSafety              = BDSGlobalConstants::Instance()->GetLengthSafety();
  vacuumSolid               = NULL;
  beamPipeSolid             = NULL;
  containerSolid            = NULL;
  containerSubtractionSolid = NULL;
  vacuumLV                  = NULL;
  beamPipeLV                = NULL;
  containerLV               = NULL;
  orientationIn  = 0;
  orientationOut = 0;
}

BDSBeamPipeFactoryCircular::~BDSBeamPipeFactoryCircular()
{
  _instance = 0;
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipe(G4String    nameIn,              // name
							G4double    lengthIn,            // length [mm]
							G4double    aper1In,             // aperture parameter 1
							G4double    /*aper2In*/,         // aperture parameter 2
							G4double    /*aper3In*/,         // aperture parameter 3
							G4double    /*aper4In*/,         // aperture parameter 4
							G4Material* vacuumMaterialIn,    // vacuum material
							G4double    beamPipeThicknessIn, // beampipe thickness [mm]
							G4Material* beamPipeMaterialIn   // beampipe material
							)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In);

  // build the solids
  vacuumSolid   = new G4Tubs(nameIn + "_vacuum_solid",      // name
			     0,                             // inner radius
			     aper1In,                       // outer radius
			     lengthIn*0.5-2*lengthSafety,   // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  beamPipeSolid = new G4Tubs(nameIn + "_pipe_solid",        // name
			     aper1In + lengthSafety,        // inner radius + length safety to avoid overlaps
			     aper1In + beamPipeThicknessIn, // outer radius
			     (lengthIn*0.5)-2*lengthSafety, // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
			      0,                            // inner radius
			      containerRadius,              // outer radius
			      (lengthIn*0.5)-lengthSafety,  // half length - must fit within magnet / outer geometry
			      0,                            // rotation start angle
			      CLHEP::twopi);                // rotation finish angle
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}



BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledIn(G4String    nameIn,              // name
								G4double    lengthIn,            // length [mm]
								G4double    angleInIn,           // the normal angle of the input face
								G4double    aper1In,             // aperture parameter 1
								G4double    /*aper2In*/,         // aperture parameter 2
								G4double    /*aper3In*/,         // aperture parameter 3
								G4double    /*aper4In */,        // aperture parameter 4
								G4Material* vacuumMaterialIn,    // vacuum material
								G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								G4Material* beamPipeMaterialIn   // beampipe material
								)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In);

  CalculateOrientations(angleInIn, 0);
  
  G4double in_z = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(fabs(angleInIn)); // note full angle here as it's the entrance angle
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(0.0, 0.0, 1.0);                    // no output face angle
  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledOut(G4String    nameIn,              // name
								 G4double    lengthIn,            // length [mm]
								 G4double    angleOutIn,           // the normal angle of the output face
								 G4double    aper1In,             // aperture parameter 1
								 G4double    /*aper2In*/,         // aperture parameter 2
								 G4double    /*aper3In*/,         // aperture parameter 3
								 G4double    /*aper4In */,        // aperture parameter 4
								 G4Material* vacuumMaterialIn,    // vacuum material
								 G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								 G4Material* beamPipeMaterialIn   // beampipe material
								 )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In);

  CalculateOrientations(0, angleOutIn);
  
  G4double out_z = cos(fabs(angleOutIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double out_x = sin(fabs(angleOutIn)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(0.0, 0.0, -1.0); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle
  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
								   G4double    lengthIn,            // length [mm]
								   G4double    angleInIn,           // the normal angle of the input face
								   G4double    angleOutIn,          // the normal angle of the input face
								   G4double    aper1In,             // aperture parameter 1
								   G4double    /*aper2In*/,         // aperture parameter 2
								   G4double    /*aper3In*/,         // aperture parameter 3
								   G4double    /*aper4In */,        // aperture parameter 4
								   G4Material* vacuumMaterialIn,    // vacuum material
								   G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								   G4Material* beamPipeMaterialIn  // beampipe material
								   )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
   // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In);

  CalculateOrientations(angleInIn, angleOutIn);
  
  G4double in_z  = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = sin(fabs(angleInIn)); // note full angle here as it's the exit angle
  G4double out_z = cos(fabs(angleOutIn));
  G4double out_x = sin(fabs(angleOutIn));
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle
  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}

/// functions below here are private to this particular factory

/// test input parameters - if not set use global defaults for this simulation
void BDSBeamPipeFactoryCircular::TestInputParameters(G4Material*&  vacuumMaterialIn,     // reference to a pointer
						     G4double&     beamPipeThicknessIn,
						     G4Material*&  beamPipeMaterialIn,
						     G4double&     aper1In)
{
  if (!vacuumMaterialIn)
    {vacuumMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());}

  if (beamPipeThicknessIn < 1e-10)
    {beamPipeThicknessIn = BDSGlobalConstants::Instance()->GetBeamPipeThickness();}

  if (!beamPipeMaterialIn)
    {beamPipeMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetBeamPipeMaterialName());}

  if (aper1In < 1e-10)
    {aper1In = BDSGlobalConstants::Instance()->GetBeamPipeRadius();}
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryCircular::CommonFinalConstruction(G4String    nameIn,
								 G4Material* vacuumMaterialIn,
								 G4Material* beamPipeMaterialIn,
								 G4double    lengthIn,
								 G4double    containerRadiusIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // prepare a longer container subtraction solid
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4Tubs(nameIn + "_container_sub_solid",// name
					 0,                              // inner radius
					 containerRadiusIn,              // outer radius
					 lengthIn,                       // full length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle
  
  
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
  
  // VISUAL ATTRIBUTES - set visual attributes
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

  // USER LIMITS
  // set user limits based on bdsim user specified parameters
#ifndef NOUSERLIMITS
  G4UserLimits* beamPipeUserLimits = new G4UserLimits("beampipe_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  beamPipeUserLimits->SetMaxAllowedStep( lengthIn * maxStepFactor );
  beamPipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  beamPipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  //attach cuts to volumes
  vacuumLV->SetUserLimits(beamPipeUserLimits);
  beamPipeLV->SetUserLimits(beamPipeUserLimits);
  containerLV->SetUserLimits(beamPipeUserLimits);
#endif

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

  // record extents
  std::pair<double,double> extX = std::make_pair(-containerRadiusIn,containerRadiusIn);
  std::pair<double,double> extY = std::make_pair(-containerRadiusIn,containerRadiusIn);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);
  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extX,extY,extZ,
				       containerSubtractionSolid,
				       vacuumLV,true,containerRadiusIn);

  // REGISTER all lvs
  aPipe->RegisterLogicalVolume(vacuumLV); //using geometry component base class method
  aPipe->RegisterLogicalVolume(beamPipeLV);
  aPipe->RegisterLogicalVolume(containerLV);

  // register sensitive volumes
  aPipe->RegisterSensitiveVolume(beamPipeLV);
  aPipe->RegisterSensitiveVolume(containerLV);
  
  return aPipe;
}


/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryCircular::CreateGeneralAngledSolids(G4String      nameIn,
							   G4double      lengthIn,
							   G4double      aper1In,
							   G4double      beamPipeThicknessIn,
							   G4ThreeVector inputfaceIn,
							   G4ThreeVector outputfaceIn)
{
  // build the solids
  vacuumSolid   = new G4CutTubs(nameIn + "_vacuum_solid",      // name
				0,                             // inner radius
				aper1In,                       // outer radius
				lengthIn*0.5-2*lengthSafety,   // half length
				0,                             // rotation start angle
				CLHEP::twopi,                  // rotation finish angle
				inputfaceIn,                   // input face normal
				outputfaceIn );                // output face normal
			              
  
  beamPipeSolid = new G4CutTubs(nameIn + "_pipe_solid",        // name
				aper1In + lengthSafety,        // inner radius + length safety to avoid overlaps
				aper1In + beamPipeThicknessIn, // outer radius
				lengthIn*0.5-2*lengthSafety,   // half length
				0,                             // rotation start angle
				CLHEP::twopi,                  // rotation finish angle
				inputfaceIn,                   // input face normal
				outputfaceIn );                // output face normal
  

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
				 0,                            // inner radius
				 aper1In + beamPipeThicknessIn + lengthSafety,  // outer radius
				 lengthIn*0.5-lengthSafety,    // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputfaceIn,                  // input face normal
				 outputfaceIn);                // rotation finish angle
}

void BDSBeamPipeFactoryCircular::CalculateOrientations(G4double angleIn, G4double angleOut)
{
  orientationIn  = BDS::CalculateOrientation(angleIn);
  orientationOut = BDS::CalculateOrientation(angleOut);
}
