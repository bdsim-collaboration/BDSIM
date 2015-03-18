#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryLHC.hh"
#include "BDSBeamPipe.hh"

#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>                         // for std::pair


BDSBeamPipeFactoryLHC* BDSBeamPipeFactoryLHC::_instance = 0;

BDSBeamPipeFactoryLHC* BDSBeamPipeFactoryLHC::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactoryLHC();}
  return _instance;
}

BDSBeamPipeFactoryLHC::BDSBeamPipeFactoryLHC()
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

BDSBeamPipeFactoryLHC::~BDSBeamPipeFactoryLHC()
{
  _instance = 0;
}

BDSBeamPipe* BDSBeamPipeFactoryLHC::CreateBeamPipe(G4String    nameIn,              // name
						   G4double    lengthIn,            // length [mm]
						   G4double    aper1In,             // rect half width
						   G4double    aper2In,             // rect half height
						   G4double    aper3In,             // radius of circle
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In);

  // build the solids
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4Tubs(nameIn + "_vacuum_cylinder", // name
				     0,                           // inner radius
				     aper3In,                     // outer radius
				     lengthIn*0.5-2*lengthSafety, // half length
				     0,                           // rotation start angle
				     CLHEP::twopi);               // rotation finish angle
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In,                // x half width
				     aper2In,                // y half width
				     lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  vacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					vacCylSolid,              // solid 1
					vacRectSolid);            // solid 2

  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* bpInnerCylSolid = new G4Tubs(nameIn + "_pipe_inner_cylinder", // name
					 0,                               // inner radius
					 aper3In + lengthSafety,          // outer radius
					 1.5*lengthIn,   // length big for unambiguous subtraction (but < outerlength)
					 0,                               // rotation start angle
					 CLHEP::twopi);                   // rotation finish angle
  //beampipe inner edge box solid (rectangular cross-section)
  G4VSolid* bpInnerRectSolid = new G4Box(nameIn + "_pipe_inner_box", // name
					 aper1In + lengthSafety,     // x half width
					 aper2In + lengthSafety,     // y half width
					 1.7*lengthIn); // z long for unambiguous intersection
  //beampipe inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* bpInnerSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpInnerCylSolid,              // solid 1
						   bpInnerRectSolid);            // solid 2

  //beampipe outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* bpOuterCylSolid = new G4Tubs(nameIn + "_pipe_inner_cylinder", // name
					 0,                               // inner radius (0 for unambiguous subtraction)
					 aper3In + beamPipeThicknessIn,   // outer radius
					 (lengthIn*0.5)-2*lengthSafety,   // half length
					 0,                               // rotation start angle
					 CLHEP::twopi);                   // rotation finish angle
  //beampipe outer edge box solid (rectangular cross-section)
  G4VSolid* bpOuterRectSolid = new G4Box(nameIn + "_pipe_inner_box",    // name
					 aper1In + beamPipeThicknessIn, // x half width
					 aper2In + beamPipeThicknessIn, // y half width
					 lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* bpOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpOuterCylSolid,              // solid 1
						   bpOuterRectSolid);            // solid 2
  

  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",  // name
					 bpOuterSolid,            // this
  					 bpInnerSolid);           // minus this
  
  //container cylindrical solid (circular cross-section)
  G4VSolid* contCylSolid = new G4Tubs(nameIn + "_vacuum_cylinder", // name
				     0,                           // inner radius
				     aper3In + beamPipeThicknessIn + lengthSafety, // outer radius
				     lengthIn*0.5, // half length
				     0,                           // rotation start angle
				     CLHEP::twopi);               // rotation finish angle
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In + beamPipeThicknessIn + lengthSafety, // x half width
				     aper2In + beamPipeThicknessIn + lengthSafety, // y half width
				     lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  containerSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					   contCylSolid,              // solid 1
					   contRectSolid);            // solid 2

  G4double width  = aper3In + beamPipeThicknessIn + lengthSafety;
  G4double height = aper2In + beamPipeThicknessIn + lengthSafety;

  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}



BDSBeamPipe* BDSBeamPipeFactoryLHC::CreateBeamPipeAngledIn(G4String    nameIn,              // name
							   G4double    lengthIn,            // length [mm]
							   G4double    angleInIn,           // the normal angle of the input face
							   G4double    aper1In,             // aperture parameter 1
							   G4double    aper2In,             // aperture parameter 2
							   G4double    aper3In,             // aperture parameter 3
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In);

  CalculateOrientations(angleInIn, 0);
  
  G4double in_z = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(fabs(angleInIn)); // note full angle here as it's the entrance angle
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(0.0, 0.0, 1.0);                    // no output face angle

  G4double width  = aper3In + beamPipeThicknessIn + lengthSafety;
  G4double height = aper2In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, beamPipeThicknessIn, inputface, outputface);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryLHC::CreateBeamPipeAngledOut(G4String    nameIn,              // name
							    G4double    lengthIn,            // length [mm]
							    G4double    angleOutIn,          // the normal angle of the output face
							    G4double    aper1In,             // aperture parameter 1
							    G4double    aper2In,             // aperture parameter 2
							    G4double    aper3In,             // aperture parameter 3
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In);

  CalculateOrientations(0, angleOutIn);
  
  G4double out_z = cos(fabs(angleOutIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double out_x = sin(fabs(angleOutIn)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(0.0, 0.0, -1.0); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle

  G4double width  = aper3In + beamPipeThicknessIn + lengthSafety;
  G4double height = aper2In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, beamPipeThicknessIn, inputface, outputface);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryLHC::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
							      G4double    lengthIn,            // length [mm]
							      G4double    angleInIn,           // the normal angle of the input face
							      G4double    angleOutIn,          // the normal angle of the input face
							      G4double    aper1In,             // aperture parameter 1
							      G4double    aper2In,             // aperture parameter 2
							      G4double    aper3In,             // aperture parameter 3
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In);

  CalculateOrientations(angleInIn, angleOutIn);
  
  G4double in_z  = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = sin(fabs(angleInIn)); // note full angle here as it's the exit angle
  G4double out_z = cos(fabs(angleOutIn));
  G4double out_x = sin(fabs(angleOutIn));
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle

  G4double width  = aper3In + beamPipeThicknessIn + lengthSafety;
  G4double height = aper2In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, beamPipeThicknessIn, inputface, outputface);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

/// functions below here are private to this particular factory

/// test input parameters - if not set use global defaults for this simulation
void BDSBeamPipeFactoryLHC::TestInputParameters(G4Material*&  vacuumMaterialIn,     // reference to a pointer
						G4double&     beamPipeThicknessIn,
						G4Material*&  beamPipeMaterialIn,
						G4double&     aper1In,
						G4double&     aper2In,
						G4double&     aper3In)
{
  if (!vacuumMaterialIn)
    {vacuumMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());}

  if (beamPipeThicknessIn < 1e-10)
    {beamPipeThicknessIn = BDSGlobalConstants::Instance()->GetBeamPipeThickness();}

  if (!beamPipeMaterialIn)
    {beamPipeMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetBeamPipeMaterialName());}

  if (aper1In < 1e-10)
    {aper1In = BDSGlobalConstants::Instance()->GetBeamPipeRadius();}

  if (aper2In < 1e-10)
    {aper2In = BDSGlobalConstants::Instance()->GetAper2();}

  if (aper3In < 1e-1)
    {aper3In = BDSGlobalConstants::Instance()->GetAper3();}
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryLHC::CommonFinalConstruction(G4String    nameIn,
							    G4Material* vacuumMaterialIn,
							    G4Material* beamPipeMaterialIn,
							    G4double    lengthIn,
							    G4double    containerWidthIn,
							    G4double    containerHeightIn)
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
  
  containerLV = new G4LogicalVolume(containerSolid,
				    vacuumMaterialIn,
				    nameIn + "_container_lv");
  
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
#ifdef BDSDEBUG
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
#else
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
#endif

  // SENSITIVITY
  // make the beampipe sensitive if required (attachd Sensitive Detector Class)
  if (BDSGlobalConstants::Instance()->GetSensitiveBeamPipe())
    {
      //beampipes are sensitive - attach appropriate sd to the beampipe volume
      beamPipeLV->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());
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
  std::pair<double,double> extX = std::make_pair(-containerWidthIn,containerWidthIn);
  std::pair<double,double> extY = std::make_pair(-containerHeightIn,containerHeightIn);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);
  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extX,extY,extZ,
				       containerSubtractionSolid,
				       vacuumLV,false,containerWidthIn);

  // REGISTER all lvs
  aPipe->RegisterLogicalVolume(vacuumLV); //using geometry component base class method
  aPipe->RegisterLogicalVolume(beamPipeLV);
  aPipe->RegisterLogicalVolume(containerLV);
  
  return aPipe;
}


/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryLHC::CreateGeneralAngledSolids(G4String      nameIn,
						      G4double      lengthIn,
						      G4double      aper1In,
						      G4double      aper2In,
						      G4double      aper3In,
						      G4double      beamPipeThicknessIn,
						      G4ThreeVector inputfaceIn,
						      G4ThreeVector outputfaceIn)
{
  
  // build the solids
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4CutTubs(nameIn + "_vacuum_cylinder", // name
					0,                           // inner radius
					aper3In,                     // outer radius
					lengthIn*0.5-2*lengthSafety, // half length
					0,                           // rotation start angle
					CLHEP::twopi,                // rotation finish angle
					inputfaceIn,                 // input face normal
					outputfaceIn);               // output face normal
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In,                // x half width
				     aper2In,                // y half width
				     lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  vacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					vacCylSolid,              // solid 1
					vacRectSolid);            // solid 2

  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* bpInnerCylSolid = new G4CutTubs(nameIn + "_pipe_inner_cylinder", // name
					    0,                               // inner radius
					    aper3In + lengthSafety,          // outer radius
					    1.5*lengthIn,   // length big for unambiguous subtraction (but < outerlength)
					    0,                               // rotation start angle
					    CLHEP::twopi,                    // rotation finish angle
					    inputfaceIn,                     // input face normal
					    outputfaceIn);                   // output face normal
  
  //beampipe inner edge box solid (rectangular cross-section)
  G4VSolid* bpInnerRectSolid = new G4Box(nameIn + "_pipe_inner_box", // name
					 aper1In + lengthSafety,     // x half width
					 aper2In + lengthSafety,     // y half width
					 1.7*lengthIn); // z long for unambiguous intersection
  //beampipe inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* bpInnerSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpInnerCylSolid,              // solid 1
						   bpInnerRectSolid);            // solid 2

  //beampipe outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* bpOuterCylSolid = new G4CutTubs(nameIn + "_pipe_inner_cylinder", // name
					    0,                               // inner radius (0 for unambiguous subtraction)
					    aper3In + beamPipeThicknessIn,   // outer radius
					    (lengthIn*0.5)-2*lengthSafety,   // half length
					    0,                               // rotation start angle
					    CLHEP::twopi,                    // rotation finish angle
					    inputfaceIn,                     // input face normal
					    outputfaceIn);                   // output face normal
  
  //beampipe outer edge box solid (rectangular cross-section)
  G4VSolid* bpOuterRectSolid = new G4Box(nameIn + "_pipe_inner_box",    // name
					 aper1In + beamPipeThicknessIn, // x half width
					 aper2In + beamPipeThicknessIn, // y half width
					 lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* bpOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpOuterCylSolid,              // solid 1
						   bpOuterRectSolid);            // solid 2
  

  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",  // name
					 bpOuterSolid,            // this
  					 bpInnerSolid);           // minus this
  
  //container cylindrical solid (circular cross-section)
  G4VSolid* contCylSolid = new G4CutTubs(nameIn + "_vacuum_cylinder", // name
					 0,                           // inner radius
					 aper3In + beamPipeThicknessIn + lengthSafety, // outer radius
					 lengthIn*0.5, // half length
					 0,                           // rotation start angle
					 CLHEP::twopi,                // rotation finish angle
					 inputfaceIn,                 // input face normal
					 outputfaceIn);               // output face normal
  
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				      aper1In + beamPipeThicknessIn + lengthSafety, // x half width
				      aper2In + beamPipeThicknessIn + lengthSafety, // y half width
				      lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  containerSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					   contCylSolid,              // solid 1
					   contRectSolid);            // solid 2
}

void BDSBeamPipeFactoryLHC::CalculateOrientations(G4double angleIn, G4double angleOut)
{
  orientationIn  = BDS::CalculateOrientation(angleIn);
  orientationOut = BDS::CalculateOrientation(angleOut);
}


void BDSBeamPipeFactoryLHC::CreateContainerSubtractionSolid(G4String& nameIn,
							    G4double& lengthIn,
							    G4double& beamPipeThicknessIn,
							    G4double& aper1In,
							    G4double& aper2In,
							    G4double& aper3In)
{
  //container cylindrical solid (circular cross-section)
  G4VSolid* contSubCylSolid = new G4Tubs(nameIn + "_subtraction_cylinder", // name
					 0,                                // inner radius
					 aper3In + beamPipeThicknessIn + lengthSafety, // outer radius
					 2*lengthIn,                       // long length for unambiguous subtraction
					 0,                                // rotation start angle
					 CLHEP::twopi);                    // rotation finish angle
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contSubRectSolid = new G4Box(nameIn + "_subtraction_box",                  // name
					 aper1In + beamPipeThicknessIn + lengthSafety, // x half width
					 aper2In + beamPipeThicknessIn + lengthSafety, // y half width
					 1.7*lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  containerSubtractionSolid = new G4IntersectionSolid(nameIn + "_subtraction_solid", // name
						      contSubCylSolid,               // solid 1
						      contSubRectSolid);             // solid 2
}
