#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryElliptical.hh"
#include "BDSBeamPipe.hh"

#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"            // for calculateorientation

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
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

#include <cmath>                           // sin, cos, fabs
#include <utility>                         // for std::pair


BDSBeamPipeFactoryElliptical* BDSBeamPipeFactoryElliptical::_instance = 0;

BDSBeamPipeFactoryElliptical* BDSBeamPipeFactoryElliptical::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactoryElliptical();}
  return _instance;
}

BDSBeamPipeFactoryElliptical::BDSBeamPipeFactoryElliptical()
{
  lengthSafety              = BDSGlobalConstants::Instance()->GetLengthSafety();
  vacuumSolid               = NULL;
  beamPipeSolid             = NULL;
  containerSolid            = NULL;
  containerSubtractionSolid = NULL;
  vacuumLV                  = NULL;
  beamPipeLV                = NULL;
  containerLV               = NULL;
  orientationIn             = 0;
  orientationOut            = 0;
}

BDSBeamPipeFactoryElliptical::~BDSBeamPipeFactoryElliptical()
{
  _instance = 0;
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipe(G4String    nameIn,              // name
							  G4double    lengthIn,            // length [mm]
							  G4double    aper1In,             // aperture parameter 1
							  G4double    aper2In,             // aperture parameter 2
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In);

  // build the solids
  vacuumSolid   = new G4EllipticalTube(nameIn + "_vacuum_solid",       // name
				       aper1In,                        // x half width
				       aper2In,                        // y half width
				       (lengthIn*0.5)-2*lengthSafety); // half length

  G4VSolid* beamPipeSolidInner; // construct rectangular beam pipe by subtracting an inner
  G4VSolid* beamPipeSolidOuter; // box from an outer one - only way
  // beamPipeSolidInner will be the inner edge of the metal beampipe
  // therefore it has to be the width of the aperture + lengthSafety
  beamPipeSolidInner = new G4EllipticalTube(nameIn + "_pipe_solid_inner",   // name
					    aper1In + lengthSafety,         // x half width - length safety to avoid overlaps
					    aper2In + lengthSafety,         // y half width
					    lengthIn);                      // length - full length fo unambiguous subtraction
  // beamPipeSolidOuter will be the outer edge of the metal beampipe
  // therefore it has to be the width of the aperture + beampipeThickness
  beamPipeSolidOuter = new G4EllipticalTube(nameIn + "_pipe_solid_outer",   // name
					    aper1In + beamPipeThicknessIn,  // x half width
					    aper2In + beamPipeThicknessIn,  // y half width
					    (lengthIn*0.5)-2*lengthSafety); // half length - lengthSafety to fit in container
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",
					 beamPipeSolidOuter,
					 beamPipeSolidInner); // outer minus inner
  
  G4double containerXHalfWidth = aper1In + beamPipeThicknessIn + lengthSafety;
  G4double containerYHalfWidth = aper2In + beamPipeThicknessIn + lengthSafety;
  containerSolid = new G4EllipticalTube(nameIn  + "_container_solid",  // name
					containerXHalfWidth,           // x half width
					containerYHalfWidth,           // y half width
					(lengthIn*0.5)-lengthSafety);  // half length
					
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn);
}



BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipeAngledIn(G4String    nameIn,              // name
								  G4double    lengthIn,            // length [mm]
								  G4double    angleInIn,           // the normal angle of the input face
								  G4double    aper1In,             // aperture parameter 1
								  G4double    aper2In,             // aperture parameter 2
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In);

  CalculateOrientations(angleInIn, 0);

  G4double in_z = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(fabs(angleInIn)); // note full angle here as it's the entrance angle
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(0.0, 0.0, 1.0);                    // no output face angle
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn);
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipeAngledOut(G4String    nameIn,              // name
								   G4double    lengthIn,            // length [mm]
								   G4double    angleOutIn,          // the normal angle of the output face
								   G4double    aper1In,             // aperture parameter 1
								   G4double    aper2In,             // aperture parameter 2
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In);

  CalculateOrientations(0, angleOutIn);
  
  G4double out_z = cos(fabs(angleOutIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double out_x = sin(fabs(angleOutIn)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(0.0, 0.0, -1.0); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn);
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
								     G4double    lengthIn,            // length [mm]
								     G4double    angleInIn,           // the normal angle of the input face
								     G4double    angleOutIn,          // the normal angle of the input face
								     G4double    aper1In,             // aperture parameter 1
								     G4double    aper2In,             // aperture parameter 2
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
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In);

  CalculateOrientations(angleInIn, angleOutIn);
  
  G4double in_z  = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = sin(fabs(angleInIn)); // note full angle here as it's the exit angle
  G4double out_z = cos(fabs(angleOutIn));
  G4double out_x = sin(fabs(angleOutIn));
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);   // no output face angle
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn);
}

/// functions below here are private to this particular factory

/// test input parameters - if not set use global defaults for this simulation
void BDSBeamPipeFactoryElliptical::TestInputParameters(G4Material*&  vacuumMaterialIn,     // reference to a pointer
						       G4double&     beamPipeThicknessIn,
						       G4Material*&  beamPipeMaterialIn,
						       G4double&     aper1In,
						       G4double&     aper2In)
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
    {aper2In = BDSGlobalConstants::Instance()->GetBeamPipeRadius();}
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryElliptical::CommonFinalConstruction(G4String    nameIn,
								   G4Material* vacuumMaterialIn,
								   G4Material* beamPipeMaterialIn,
								   G4double    lengthIn,
								   G4double    aper1In,
								   G4double    aper2In,
								   G4double    beamPipeThicknessIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // prepare a longer container subtraction solid
  G4double containerXHalfWidth = aper1In + beamPipeThicknessIn + lengthSafety;
  G4double containerYHalfWidth = aper2In + beamPipeThicknessIn + lengthSafety;
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4EllipticalTube(nameIn  + "_container_solid", // name
						   containerXHalfWidth,          // x half width
						   containerYHalfWidth,          // y half width
						   lengthIn);                    // full length for unambiguous subtraction
  
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
  
  // USER LIMITS - set user limits based on bdsim user specified parameters
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
  std::pair<double,double> extX = std::make_pair(-containerXHalfWidth,containerXHalfWidth);
  std::pair<double,double> extY = std::make_pair(-containerYHalfWidth,containerYHalfWidth);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);
  // calculate radius if a tube were to be place around it
  G4double containerRadius = sqrt(containerXHalfWidth*containerXHalfWidth + containerYHalfWidth*containerYHalfWidth);
  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extX,extY,extZ,
				       containerSubtractionSolid,
				       vacuumLV,false,containerRadius);

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
void BDSBeamPipeFactoryElliptical::CreateGeneralAngledSolids(G4String      nameIn,
							     G4double      lengthIn,
							     G4double      aper1In,
							     G4double      aper2In,
							     G4double      beamPipeThicknessIn,
							     G4ThreeVector inputfaceIn,
							     G4ThreeVector outputfaceIn)
{
  // this function will make a longer normal rectangular beampipe and chop it off
  // to make angled faces as required
  // achieve this using the intersection of the normal beampipe (but a little longer)
  // with a large G4CutTubs to get the angled faces.
  // note even if one face is flat, we don't save a boolean operation as the intersection
  // can be on both sides using a G4CutTubs.  Also, keeping one side flat would require
  // shifting the volume from 0 which causes headaches later with SDs.

  // build the solids - vacuum, beampipe and container solids
  // extra solids required for booleans
  G4VSolid* vacuumSolidLong;
  G4VSolid* beamPipeSolidLong;
  G4VSolid* angledFaceSolid;
  G4VSolid* containerSolidLong;
  G4VSolid* angledFaceSolidContainer;

  // build the solid with angled faces for intersection
  G4double angledFaceRadius = (std::max(aper1In,aper2In) + beamPipeThicknessIn)*2.0; //huge for unambiguous intersection
  angledFaceSolid = new G4CutTubs(nameIn + "_angled_face",       // name
				  0,                             // inner radius
				  angledFaceRadius,              // outer radius
				  (lengthIn*0.5)-2*lengthSafety, // half length - must fit within container
				  0,                             // rotation start angle
				  CLHEP::twopi,                  // rotation finish angle
				  inputfaceIn,                   // input face normal
				  outputfaceIn);                 // output face normal
  
  vacuumSolidLong = new G4EllipticalTube(nameIn + "_vacuum_solid_long", // name
					 aper1In,                       // x half width
					 aper2In,                       // y half width
					 lengthIn);                     // full length for unambiguous boolean
  vacuumSolid     = new G4IntersectionSolid(nameIn + "_vacuum_solid",
					    vacuumSolidLong,
					    angledFaceSolid);
  
  G4VSolid* beamPipeSolidInner; // construct rectangular beam pipe by subtracting an inner
  G4VSolid* beamPipeSolidOuter; // box from an outer one - only way
  // beamPipeSolidInner will be the inner edge of the metal beampipe
  // therefore it has to be the width of the aperture + lengthSafety
  beamPipeSolidInner = new G4EllipticalTube(nameIn + "_pipe_solid_inner",   // name
					    aper1In + lengthSafety,         // x half width - length safety to avoid overlaps
					    aper2In + lengthSafety,         // y half width
					    2*lengthIn);                    // 2*length - full length fo unambiguous subtraction
  // beamPipeSolidOuter will be the outer edge of the metal beampipe
  // therefore it has to be the width of the aperture + beampipeThickness
  beamPipeSolidOuter = new G4EllipticalTube(nameIn + "_pipe_solid_outer",   // name
					    aper1In + beamPipeThicknessIn,  // x half width
					    aper2In + beamPipeThicknessIn,  // y half width
					    lengthIn);                      // full length for unambiguous intersection
  beamPipeSolidLong = new G4SubtractionSolid(nameIn + "_pipe_solid_long",
					 beamPipeSolidOuter,
					 beamPipeSolidInner); // outer minus inner
  beamPipeSolid = new G4IntersectionSolid(nameIn + "_pipe_solid",
					  beamPipeSolidLong,
					  angledFaceSolid);
  
  G4double containerXHalfWidth = aper1In + beamPipeThicknessIn + lengthSafety;
  G4double containerYHalfWidth = aper2In + beamPipeThicknessIn + lengthSafety;
  containerSolidLong = new G4EllipticalTube(nameIn  + "_container_solid_long",// name
					    containerXHalfWidth,              // x half width
					    containerYHalfWidth,              // y half width
					    lengthIn);                        // full length for unambiguous intersection
  angledFaceSolidContainer = new G4CutTubs(nameIn + "_angled_face_container",// name
					   0,                                // inner radius
					   angledFaceRadius,                 // outer radius
					   (lengthIn*0.5)-lengthSafety,      // half length - must fit within magnet
					   0,                                // rotation start angle
					   CLHEP::twopi,                     // rotation finish angle
					   inputfaceIn,                      // input face normal
					   outputfaceIn);                    // output face normal
  containerSolid = new G4IntersectionSolid(nameIn + "_container_solid",
					   containerSolidLong,
					   angledFaceSolidContainer);
}

void BDSBeamPipeFactoryElliptical::CalculateOrientations(G4double angleIn, G4double angleOut)
{
  orientationIn  = BDS::CalculateOrientation(angleIn);
  orientationOut = BDS::CalculateOrientation(angleOut);
}
