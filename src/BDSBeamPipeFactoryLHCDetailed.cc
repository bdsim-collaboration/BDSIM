#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
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


BDSBeamPipeFactoryLHCDetailed* BDSBeamPipeFactoryLHCDetailed::_instance = 0;

BDSBeamPipeFactoryLHCDetailed* BDSBeamPipeFactoryLHCDetailed::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactoryLHCDetailed();}
  return _instance;
}

BDSBeamPipeFactoryLHCDetailed::BDSBeamPipeFactoryLHCDetailed()
{
  lengthSafety              = BDSGlobalConstants::Instance()->GetLengthSafety();
  coldBoreThickness         = 1.5*CLHEP::mm;
  coolingPipeThickness      = 0.53*CLHEP::mm;
  coolingPipeRadius         = 3.7*CLHEP::mm; // will be overwritten if needs be to fit inside beampipe
  coolingPipeYOffset        = 0.0;  //initialised only
  copperSkinThickness       = 75*CLHEP::um;
  vacuumSolid               = NULL; // the inner vacuum shape
  screenSolid               = NULL; // the beam screen (first bit of aperture)
  coolingPipeSolid          = NULL; // small cooling pipe above and below beam screen
  beamPipeSolid             = NULL; // outer pipe between vacuum and cold bore
  containerSolid            = NULL;
  containerSubtractionSolid = NULL;
  vacuumLV                  = NULL;
  screenLV                  = NULL;
  coolingPipeLV             = NULL;
  beamPipeLV                = NULL;
  containerLV               = NULL;
  orientationIn  = 0;
  orientationOut = 0;
}

BDSBeamPipeFactoryLHCDetailed::~BDSBeamPipeFactoryLHCDetailed()
{
  _instance = 0;
}

G4double BDSBeamPipeFactoryLHCDetailed::GetFullWidthOfCoolingPipe()
{
  return coolingPipeRadius + coolingPipeThickness + 2*lengthSafety;
}  

BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CreateBeamPipe(G4String    nameIn,              // name
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


  //copper skin layer to beamscreen
  //copper skin inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* cuInnerCylSolid = new G4Tubs(nameIn + "_cu_inner_cylinder", // name
					 0,                             // inner radius
					 aper3In + 1*CLHEP::um,         // outer radius
					 1.5*lengthIn,   // length big for unambiguous subtraction (but < outerlength)
					 0,                             // rotation start angle
					 CLHEP::twopi);                 // rotation finish angle
  //screen inner edge box solid (rectangular cross-section)
  G4VSolid* cuInnerRectSolid = new G4Box(nameIn + "_cu_inner_box", // name
					 aper1In + 1*CLHEP::um,    // x half width
					 aper2In + 1*CLHEP::um,    // y half width
					 1.7*lengthIn); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* cuInnerSolid = new G4IntersectionSolid(nameIn + "_cu_inner_solid", // name
						   cuInnerCylSolid,            // solid 1
						   cuInnerRectSolid);          // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* cuOuterCylSolid = new G4Tubs(nameIn + "_cu_outer_cylinder",    // name
					 0,                                // inner radius (0 for unambiguous subtraction)
					 aper3In + copperSkinThickness,    // outer radius
					 (lengthIn*0.5)-2*lengthSafety,    // half length
					 0,                                // rotation start angle
					 CLHEP::twopi);                    // rotation finish angle
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* cuOuterRectSolid = new G4Box(nameIn + "_cu_outer_box",      // name
					 aper1In + copperSkinThickness, // x half width
					 aper2In + copperSkinThickness, // y half width
					 lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* cuOuterSolid = new G4IntersectionSolid(nameIn + "_cu_outer_solid", // name
						   cuOuterCylSolid,            // solid 1
						   cuOuterRectSolid);          // solid 2
  copperSkinSolid = new G4SubtractionSolid(nameIn + "_copper_skin_solid",// name
					   cuOuterSolid,                 // this
					   cuInnerSolid);                // minus this
  
  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  //using large margin of 1um here to avoid misplacement errors with compound booleans in geant4 (empirical solution)
  G4VSolid* screenInnerCylSolid = new G4Tubs(nameIn + "_screen_inner_cylinder", // name
					     0,                                 // inner radius
					     aper3In + copperSkinThickness + 1*CLHEP::um, // outer radius
					     1.5*lengthIn,   // length big for unambiguous subtraction (but < outerlength)
					     0,                                 // rotation start angle
					     CLHEP::twopi);                     // rotation finish angle
  //screen inner edge box solid (rectangular cross-section)
  //using large margin of 1um here to avoid misplacement errors with compound booleans in geant4 (empirical solution)
  G4VSolid* screenInnerRectSolid = new G4Box(nameIn + "_screen_inner_box", // name
					     aper1In + copperSkinThickness + 1*CLHEP::um, // x half width
					     aper2In + copperSkinThickness + 1*CLHEP::um, // y half width
					     1.7*lengthIn); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* screenInnerSolid = new G4IntersectionSolid(nameIn + "_screen_inner_solid", // name
						       screenInnerCylSolid,            // solid 1
						       screenInnerRectSolid);          // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* screenOuterCylSolid = new G4Tubs(nameIn + "_screen_outer_cylinder",// name
					     0,                                // inner radius (0 for unambiguous subtraction)
					     aper3In + copperSkinThickness + 2*lengthSafety + beamPipeThicknessIn, // outer radius
					     (lengthIn*0.5)-2*lengthSafety,    // half length
					     0,                                // rotation start angle
					     CLHEP::twopi);                    // rotation finish angle
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* screenOuterRectSolid = new G4Box(nameIn + "_screen_outer_box", // name
					     aper1In + copperSkinThickness + 2*lengthSafety + beamPipeThicknessIn, // x half width
					     aper2In + copperSkinThickness + 2*lengthSafety + beamPipeThicknessIn, // y half width
					     lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* screenOuterSolid = new G4IntersectionSolid(nameIn + "_screen__outersolid", // name
						       screenOuterCylSolid,            // solid 1
						       screenOuterRectSolid);          // solid 2
  screenSolid = new G4SubtractionSolid(nameIn + "_screen_solid",// name
				       screenOuterSolid,        // this
				       screenInnerSolid);       // minus this

  coolingPipeYOffset = aper2In + lengthSafety + copperSkinThickness + lengthSafety + beamPipeThicknessIn +lengthSafety + coolingPipeThickness + coolingPipeRadius;
  
  coolingPipeSolid = new G4Tubs(nameIn + "_cooling_pipe_solid",           // name
				coolingPipeRadius,                        // inner radius
				coolingPipeRadius + coolingPipeThickness, // outer radius
				(lengthIn*0.5)-2*lengthSafety,            // half length
				0,                                        // rotation start angle
				CLHEP::twopi);                            // rotation finish angle

  G4double coldBoreRadius = aper3In + copperSkinThickness + beamPipeThicknessIn + 1*CLHEP::um + 4*lengthSafety;
  // beampipe - ("coldbore") circular cross-section and sits outisde screen and cooling pipe
  beamPipeSolid = new G4Tubs(nameIn + "_beampipe_solid",         // name
			     coldBoreRadius,                     // inner radius
			     coldBoreRadius + coldBoreThickness, // outer radius
			     (lengthIn*0.5)-2*lengthSafety,      // half length
			     0,                                  // rotation start angle
			     CLHEP::twopi);                      // rotation finish angle
  
  //container cylindrical solid (circular cross-section)
  G4double containerRadius = coldBoreRadius+ coldBoreThickness + 1*CLHEP::um;
  containerSolid = new G4Tubs(nameIn + "_container_cylinder", // name
			      0,                              // inner radius
			      containerRadius,                // outer radius
			      lengthIn*0.5,                   // half length
			      0,                              // rotation start angle
			      CLHEP::twopi);                  // rotation finish angle

  //container cylindrical solid (circular cross-section)
  containerSubtractionSolid = new G4Tubs(nameIn + "_subtraction_cylinder", // name
					 0,                                // inner radius
					 containerRadius + lengthSafety,   // outer radius
					 2*lengthIn,                       // long length for unambiguous subtraction
					 0,                                // rotation start angle
					 CLHEP::twopi);                    // rotation finish angle
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}



BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CreateBeamPipeAngledIn(G4String    nameIn,              // name
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
  G4double containerRadius = aper3In + beamPipeThicknessIn + lengthSafety + coldBoreThickness + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CreateBeamPipeAngledOut(G4String    nameIn,              // name
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
  G4double containerRadius = aper3In + beamPipeThicknessIn + lengthSafety + coldBoreThickness + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
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
  G4double containerRadius = aper3In + beamPipeThicknessIn + lengthSafety + coldBoreThickness + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
}

/// functions below here are private to this particular factory

/// test input parameters - if not set use global defaults for this simulation
void BDSBeamPipeFactoryLHCDetailed::TestInputParameters(G4Material*&  vacuumMaterialIn,     // reference to a pointer
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
BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CommonFinalConstruction(G4String    nameIn,
								    G4Material* vacuumMaterialIn,
								    G4Material* beamPipeMaterialIn,
								    G4double    lengthIn,
								    G4double    containerRadiusIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // get materials
  G4Material* copper = BDSMaterials::Instance()->GetMaterial("Copper");

  
  // build the logical volumes
  vacuumLV      = new G4LogicalVolume(vacuumSolid,
				      vacuumMaterialIn,
				      nameIn + "_vacuum_lv");

  copperSkinLV  = new G4LogicalVolume(copperSkinSolid,
				      copper,
				      nameIn + "_copper_lv");
  
  screenLV      = new G4LogicalVolume(screenSolid,
				      beamPipeMaterialIn,
				      nameIn + "_screen_lv");
  
  coolingPipeLV = new G4LogicalVolume(coolingPipeSolid,
				      beamPipeMaterialIn,
				      nameIn + "_cooling_pipe_lv");
  
  beamPipeLV    = new G4LogicalVolume(beamPipeSolid,
				      beamPipeMaterialIn,
				      nameIn + "_beampipe_lv");
  
  containerLV   = new G4LogicalVolume(containerSolid,
				      vacuumMaterialIn,
				      nameIn + "_container_lv");
  
  // VISUAL ATTRIBUTES
  // set visual attributes
  // copper skin
  G4VisAttributes* cuVisAttr   = new G4VisAttributes(G4Colour(0.722, 0.525, 0.043));
  cuVisAttr->SetForceLineSegmentsPerCircle(50);
  cuVisAttr->SetVisibility(true);
  cuVisAttr->SetForceSolid(true);

  // beampipe
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(G4Colour(0.4,0.4,0.4));
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceSolid(true);
  pipeVisAttr->SetForceLineSegmentsPerCircle(60);
  
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  copperSkinLV->SetVisAttributes(cuVisAttr);
  screenLV->SetVisAttributes(pipeVisAttr);
  coolingPipeLV->SetVisAttributes(pipeVisAttr);
  beamPipeLV->SetVisAttributes(pipeVisAttr);
  // container
#ifdef BDSDEBUG
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
#else
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
#endif

  // USER LIMITS - set user limits based on bdsim user specified parameters
#ifndef NOUSERLIMITS
  G4UserLimits* beamPipeUserLimits = new G4UserLimits("beampipe_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  beamPipeUserLimits->SetMaxAllowedStep( lengthIn * maxStepFactor );
  beamPipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  beamPipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  //attach cuts to volumes
  vacuumLV->SetUserLimits(beamPipeUserLimits);
  copperSkinLV->SetUserLimits(beamPipeUserLimits);
  screenLV->SetUserLimits(beamPipeUserLimits);
  coolingPipeLV->SetUserLimits(beamPipeUserLimits);
  beamPipeLV->SetUserLimits(beamPipeUserLimits);
  containerLV->SetUserLimits(beamPipeUserLimits);
#endif

  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    G4ThreeVector(0,0,0),         // position
		    vacuumLV,                     // lv to be placed
		    nameIn + "_vacuum_pv",        // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    G4ThreeVector(0,0,0),         // position
		    copperSkinLV,                 // lv to be placed
		    nameIn + "_copper_skin_pv",   // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    screenLV,                     // lv to be placed
		    nameIn + "_screen_pv",        // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  
  
  G4ThreeVector* coolingPipeTopPosition    = new G4ThreeVector(0,coolingPipeYOffset,0);
  G4ThreeVector* coolingPipeBottomPosition = new G4ThreeVector(0,-coolingPipeYOffset,0);
  
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    *coolingPipeTopPosition,      // position
		    coolingPipeLV,                // lv to be placed
		    nameIn + "_cooling_pipe_top_pv", // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    *coolingPipeBottomPosition,   // position
		    coolingPipeLV,                // lv to be placed
		    nameIn + "_cooling_pipe_bottom_pv", // name
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
  aPipe->RegisterLogicalVolume(screenLV);
  aPipe->RegisterLogicalVolume(coolingPipeLV);
  aPipe->RegisterLogicalVolume(beamPipeLV);
  aPipe->RegisterLogicalVolume(containerLV);
  aPipe->RegisterLogicalVolume(copperSkinLV);

  // register sensitive volumes
  aPipe->RegisterSensitiveVolume(screenLV);
  aPipe->RegisterSensitiveVolume(coolingPipeLV);
  aPipe->RegisterSensitiveVolume(beamPipeLV);
  aPipe->RegisterSensitiveVolume(containerLV);
  aPipe->RegisterSensitiveVolume(copperSkinLV);

  
  return aPipe;
}


/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryLHCDetailed::CreateGeneralAngledSolids(G4String      nameIn,
							      G4double      lengthIn,
							      G4double      aper1In,
							      G4double      aper2In,
							      G4double      aper3In,
							      G4double      beamPipeThicknessIn,
							      G4ThreeVector inputfaceIn,
							      G4ThreeVector outputfaceIn)
{

  #ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
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


  //copper skin layer to beamscreen
  //copper skin inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* cuInnerCylSolid = new G4CutTubs(nameIn + "_cu_inner_cylinder", // name
					    0,                             // inner radius
					    aper3In + 1*CLHEP::um,         // outer radius
					    1.5*lengthIn,   // length big for unambiguous subtraction (but < outerlength)
					    0,                             // rotation start angle
					    CLHEP::twopi,                  // rotation finish angle
					    inputfaceIn,                   // input face normal
					    outputfaceIn);                 // output face normal
  //screen inner edge box solid (rectangular cross-section)
  G4VSolid* cuInnerRectSolid = new G4Box(nameIn + "_cu_inner_box", // name
					 aper1In + 1*CLHEP::um,    // x half width
					 aper2In + 1*CLHEP::um,    // y half width
					 1.7*lengthIn); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* cuInnerSolid = new G4IntersectionSolid(nameIn + "_cu_inner_solid", // name
						   cuInnerCylSolid,            // solid 1
						   cuInnerRectSolid);          // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* cuOuterCylSolid = new G4CutTubs(nameIn + "_cu_outer_cylinder",    // name
					    0,                                // inner radius (0 for unambiguous subtraction)
					    aper3In + copperSkinThickness,    // outer radius
					    (lengthIn*0.5)-2*lengthSafety,    // half length
					    0,                                // rotation start angle
					    CLHEP::twopi,                     // rotation finish angle
					    inputfaceIn,                      // input face normal
					    outputfaceIn);                    // output face normal
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* cuOuterRectSolid = new G4Box(nameIn + "_cu_outer_box",      // name
					 aper1In + copperSkinThickness, // x half width
					 aper2In + copperSkinThickness, // y half width
					 lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* cuOuterSolid = new G4IntersectionSolid(nameIn + "_cu_outer_solid", // name
						   cuOuterCylSolid,            // solid 1
						   cuOuterRectSolid);          // solid 2
  copperSkinSolid = new G4SubtractionSolid(nameIn + "_copper_skin_solid",// name
					   cuOuterSolid,                 // this
					   cuInnerSolid);                // minus this
  
  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* screenInnerCylSolid = new G4CutTubs(nameIn + "_screen_inner_cylinder", // name
						0,                                 // inner radius
						aper3In + copperSkinThickness + 1*CLHEP::um, // outer radius
						1.5*lengthIn,   // length big for unambiguous subtraction (but < outerlength)
						0,                                 // rotation start angle
						CLHEP::twopi,                      // rotation finish angle
						inputfaceIn,                       // input face normal
						outputfaceIn);                     // output face normal
  //screen inner edge box solid (rectangular cross-section)
  G4VSolid* screenInnerRectSolid = new G4Box(nameIn + "_screen_inner_box", // name
					     aper1In + copperSkinThickness + 1*CLHEP::um, // x half width
					     aper2In + copperSkinThickness + 1*CLHEP::um, // y half width
					     1.7*lengthIn); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* screenInnerSolid = new G4IntersectionSolid(nameIn + "_screen_inner_solid", // name
						       screenInnerCylSolid,            // solid 1
						       screenInnerRectSolid);          // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* screenOuterCylSolid = new G4CutTubs(nameIn + "_screen_outer_cylinder",// name
						0,                                // inner radius (0 for unambiguous subtraction)
						aper3In + copperSkinThickness + 2*lengthSafety + beamPipeThicknessIn, // outer radius
						(lengthIn*0.5)-2*lengthSafety,    // half length
						0,                                // rotation start angle
						CLHEP::twopi,                     // rotation finish angle
						inputfaceIn,                      // input face normal
						outputfaceIn);                    // output face normal
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* screenOuterRectSolid = new G4Box(nameIn + "_screen_outer_box", // name
					     aper1In + copperSkinThickness + 2*lengthSafety + beamPipeThicknessIn, // x half width
					     aper2In + copperSkinThickness + 2*lengthSafety + beamPipeThicknessIn, // y half width
					     lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* screenOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_outer_solid", // name
						       screenOuterCylSolid,          // solid 1
						       screenOuterRectSolid);        // solid 2
  

  screenSolid = new G4SubtractionSolid(nameIn + "_screen_solid",// name
				       screenOuterSolid,        // this
				       screenInnerSolid);       // minus this

  coolingPipeYOffset = aper2In + lengthSafety + copperSkinThickness + lengthSafety + beamPipeThicknessIn +lengthSafety + coolingPipeThickness + coolingPipeRadius;

  
  coolingPipeSolid = new G4CutTubs(nameIn + "_cooling_pipe_solid",           // name
				   coolingPipeRadius,                        // inner radius
				   coolingPipeRadius + coolingPipeThickness, // outer radius
				   (lengthIn*0.5)-2*lengthSafety,            // half length
				   0,                                        // rotation start angle
				   CLHEP::twopi,                             // rotation finish angle
				   inputfaceIn,                              // input face normal
				   outputfaceIn);                            // output face normal

  G4double coldBoreRadius = aper3In + copperSkinThickness + beamPipeThicknessIn + 1*CLHEP::um + 4*lengthSafety;
  // beampipe - ("coldbore") circular cross-section and sits outisde screen and cooling pipe
  beamPipeSolid = new G4CutTubs(nameIn + "_beampipe_solid",         // name
				coldBoreRadius,                     // inner radius
				coldBoreRadius + coldBoreThickness, // outer radius
				(lengthIn*0.5)-2*lengthSafety,      // half length
				0,                                  // rotation start angle
				CLHEP::twopi,                       // rotation finish angle
				inputfaceIn,                        // input face normal
				outputfaceIn);                      // output face normal

  //container cylindrical solid (circular cross-section)
  G4double containerRadius = coldBoreRadius + coldBoreThickness + 1*CLHEP::um;
  containerSolid = new G4Tubs(nameIn + "_container_cylinder", // name
			      0,                              // inner radius
			      containerRadius,                // outer radius
			      lengthIn*0.5,                   // half length
			      0,                              // rotation start angle
			      CLHEP::twopi);                  // rotation finish angle

  //container cylindrical solid (circular cross-section)
  containerSubtractionSolid = new G4Tubs(nameIn + "_subtraction_cylinder", // name
					 0,                                // inner radius
					 containerRadius + lengthSafety,   // outer radius
					 2*lengthIn,                       // long length for unambiguous subtraction
					 0,                                // rotation start angle
					 CLHEP::twopi);                    // rotation finish angle
}

void BDSBeamPipeFactoryLHCDetailed::CalculateOrientations(G4double angleIn, G4double angleOut)
{
  orientationIn  = BDS::CalculateOrientation(angleIn);
  orientationOut = BDS::CalculateOrientation(angleOut);
}
