#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryRectEllipse.hh"
#include "BDSBeamPipe.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
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

BDSBeamPipeFactoryRectEllipse* BDSBeamPipeFactoryRectEllipse::_instance = 0;

BDSBeamPipeFactoryRectEllipse* BDSBeamPipeFactoryRectEllipse::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactoryRectEllipse();}
  return _instance;
}

BDSBeamPipeFactoryRectEllipse::BDSBeamPipeFactoryRectEllipse():BDSBeamPipeFactoryBase()
{
}

BDSBeamPipeFactoryRectEllipse::~BDSBeamPipeFactoryRectEllipse()
{
  _instance = 0;
}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CreateBeamPipe(G4String    nameIn,              // name
							   G4double    lengthIn,            // length [mm]
							   G4double    aper1In,             // rect half width
							   G4double    aper2In,             // rect half height
							   G4double    aper3In,             // radius of circle
							   G4double    aper4In,             // aperture parameter 4
							   G4Material* vacuumMaterialIn,    // vacuum material
							   G4double    beamPipeThicknessIn, // beampipe thickness [mm]
							   G4Material* beamPipeMaterialIn   // beampipe material
							   )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In,aper4In);

  // build the solids
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid", // name
					       aper3In,                      // horizontal semi-axis
					       aper4In,                      // vertical semi-axis
					       lengthIn*0.5-2*lengthSafety); // half length
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In,                // x half width
				     aper2In,                // y half width
				     lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  vacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					vacCylSolid,              // solid 1
					vacRectSolid);            // solid 2

  //beampipe solid
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* bpInnerCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid",// name
						   aper3In + lengthSafety,          // horizontal semi-axis
						   aper4In + lengthSafety,          // vertical semi-axis
						   1.5*lengthIn); // length big for unambiguous subtraction (but < outerlength)
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
  G4VSolid* bpOuterCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid",// name
						   aper3In + beamPipeThicknessIn,   // horizontal semi-axis
						   aper4In + beamPipeThicknessIn,   // hotizontal semi-axis
						   (lengthIn*0.5)-2*lengthSafety);  // half length
  //beampipe outer edge box solid (rectangular cross-section)
  G4VSolid* bpOuterRectSolid = new G4Box(nameIn + "_pipe_inner_box",    // name
					 aper1In + beamPipeThicknessIn, // x half width
					 aper2In + beamPipeThicknessIn, // y half width
					 lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* bpOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpOuterCylSolid,              // solid 1
						   bpOuterRectSolid);            // solid 2
  //beampipe final subtraction between outer and inner edge
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",  // name
					 bpOuterSolid,            // this
  					 bpInnerSolid);           // minus this
  
  //container cylindrical solid (circular cross-section)
  G4VSolid* contCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid", // name
						aper3In + beamPipeThicknessIn + lengthSafety, // horizontal semi-axis
						aper4In + beamPipeThicknessIn + lengthSafety, // vertical semi-axis
						lengthIn*0.5); // half length
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contRectSolid = new G4Box(nameIn + "_vacuum_box",                       // name
				      aper1In + beamPipeThicknessIn + lengthSafety, // x half width
				      aper2In + beamPipeThicknessIn + lengthSafety, // y half width
				      lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  containerSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					   contCylSolid,             // solid 1
					   contRectSolid);           // solid 2

  G4double width  = aper3In + beamPipeThicknessIn + lengthSafety;
  G4double height = aper2In + beamPipeThicknessIn + lengthSafety;

  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In, aper4In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CreateBeamPipeAngledIn(G4String    nameIn,              // name
								   G4double    lengthIn,            // length [mm]
								   G4double    angleInIn,           // the normal angle of the input face
								   G4double    aper1In,             // aperture parameter 1
								   G4double    aper2In,             // aperture parameter 2
								   G4double    aper3In,             // aperture parameter 3
								   G4double    aper4In,             // aperture parameter 4
								   G4Material* vacuumMaterialIn,    // vacuum material
								   G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								   G4Material* beamPipeMaterialIn   // beampipe material
								   )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In, aper4In);

  CalculateOrientations(angleInIn, 0);
  
  G4double in_z = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(fabs(angleInIn)); // note full angle here as it's the entrance angle
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(0.0, 0.0, 1.0);                    // no output face angle

  G4double width  = std::max(aper1In,aper3In) + beamPipeThicknessIn + lengthSafety;
  G4double height = std::max(aper2In,aper4In) + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, aper4In, beamPipeThicknessIn, inputface, outputface);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In, aper4In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CreateBeamPipeAngledOut(G4String    nameIn,              // name
								    G4double    lengthIn,            // length [mm]
								    G4double    angleOutIn,          // the normal angle of the output face
								    G4double    aper1In,             // aperture parameter 1
								    G4double    aper2In,             // aperture parameter 2
								    G4double    aper3In,             // aperture parameter 3
								    G4double    aper4In,             // aperture parameter 4
								    G4Material* vacuumMaterialIn,    // vacuum material
								    G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								    G4Material* beamPipeMaterialIn   // beampipe material
								    )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In,aper4In);

  CalculateOrientations(0, angleOutIn);
  
  G4double out_z = cos(fabs(angleOutIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double out_x = sin(fabs(angleOutIn)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(0.0, 0.0, -1.0); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle

  G4double width  = std::max(aper1In,aper3In) + beamPipeThicknessIn + lengthSafety;
  G4double height = std::max(aper2In,aper4In) + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, aper4In, beamPipeThicknessIn, inputface, outputface);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In, aper4In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
								      G4double    lengthIn,            // length [mm]
								      G4double    angleInIn,           // the normal angle of the input face
								      G4double    angleOutIn,          // the normal angle of the input face
								      G4double    aper1In,             // aperture parameter 1
								      G4double    aper2In,             // aperture parameter 2
								      G4double    aper3In,             // aperture parameter 3
								      G4double    aper4In,             // aperture parameter 4
								      G4Material* vacuumMaterialIn,    // vacuum material
								      G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								      G4Material* beamPipeMaterialIn   // beampipe material
								      )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
   // test input parameters - set global options as default if not specified
  TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn,aper1In,aper2In,aper3In,aper4In);

  CalculateOrientations(angleInIn, angleOutIn);
  
  G4double in_z  = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = sin(fabs(angleInIn)); // note full angle here as it's the exit angle
  G4double out_z = cos(fabs(angleOutIn));
  G4double out_x = sin(fabs(angleOutIn));
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);               // no output face angle

  G4double width  = std::max(aper1In,aper3In) + beamPipeThicknessIn + lengthSafety;
  G4double height = std::max(aper2In,aper4In) + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, aper4In, beamPipeThicknessIn, inputface, outputface);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In, aper4In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

/// functions below here are private to this particular factory

/// test input parameters - if not set use global defaults for this simulation
void BDSBeamPipeFactoryRectEllipse::TestInputParameters(G4Material*&  vacuumMaterialIn,     // reference to a pointer
							G4double&     beamPipeThicknessIn,
							G4Material*&  beamPipeMaterialIn,
							G4double&     aper1In,
							G4double&     aper2In,
							G4double&     aper3In,
							G4double&     aper4In)
{
    BDSBeamPipeFactoryBase::TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn);

  if (aper1In < 1e-10)
    {aper1In = BDSGlobalConstants::Instance()->GetBeamPipeRadius();}

  if (aper2In < 1e-10)
    {aper2In = BDSGlobalConstants::Instance()->GetAper2();}

  if (aper3In < 1e-1)
    {aper3In = BDSGlobalConstants::Instance()->GetAper3();}

  if (aper4In < 1e-1)
    {aper4In = BDSGlobalConstants::Instance()->GetAper4();}
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CommonFinalConstruction(G4String    nameIn,
								    G4Material* vacuumMaterialIn,
								    G4Material* beamPipeMaterialIn,
								    G4double    lengthIn,
								    G4double    containerWidthIn,
								    G4double    containerHeightIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBeamPipeFactoryBase::CommonConstruction(nameIn,
					     vacuumMaterialIn,
					     beamPipeMaterialIn,
					     lengthIn);
  
  // record extents
  std::pair<double,double> extX = std::make_pair(-containerWidthIn,containerWidthIn);
  std::pair<double,double> extY = std::make_pair(-containerHeightIn,containerHeightIn);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);
  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = BuildBeamPipeAndRegisterVolumes(extX,extY,extZ,containerWidthIn);
  return aPipe;
}

/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryRectEllipse::CreateGeneralAngledSolids(G4String      nameIn,
							      G4double      lengthIn,
							      G4double      aper1In,
							      G4double      aper2In,
							      G4double      aper3In,
							      G4double      aper4In,
							      G4double      beamPipeThicknessIn,
							      G4ThreeVector inputfaceIn,
							      G4ThreeVector outputfaceIn)
{
  // build the solids
  // we can get the rectangular ellipse as in the straight case with the intersection of
  // an elliptical tube (always solid) and then we can use another intersection solid
  // with a larger (wider and taller) G4CutTubs to get the angled faces.
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid",// name
					       aper3In,                     // horizontal semi-axis
					       aper4In,                     // vertical semi-axis
					       lengthIn); // z full width (long for unambiguous intersection)
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In,                // x half width
				     aper2In,                // y half width
				     lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  G4VSolid* longVacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_long_solid", // name
						      vacCylSolid,                   // solid 1
						      vacRectSolid);                 // solid 2
  //prepare angled face large cylinder for intersection get angled faces
  //we can actually use this for the beampipe too later on - whew
  G4double angledFaceRadius = (std::max(std::max(aper1In,aper2In),std::max(aper3In,aper4In)) + beamPipeThicknessIn) * 4;
  G4VSolid* vacuumAngledSolid = new G4CutTubs(nameIn + "_pipe_angled_faces",     // name
					      0,                                 // inner radius
					      angledFaceRadius,                  // outer radius
					      (lengthIn*0.5) - (2*lengthSafety), // accurate length
					      0,                                 // rotation start angle
					      CLHEP::twopi,                      // rotation sweep angle
					      inputfaceIn,                       // input face normal
					      outputfaceIn);                     // output face normal
  vacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					longVacuumSolid,          // solid 1
					vacuumAngledSolid);       // solid 2

  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* bpInnerCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid", // name
						   aper3In + lengthSafety,          // horizontal semi-axis
						   aper4In + lengthSafety,          // vertical semi-axis
						   1.5*lengthIn); // length big for unambiguous subtraction (but < outerlength)
  //beampipe inner edge box solid (rectangular cross-section)
  G4VSolid* bpInnerRectSolid = new G4Box(nameIn + "_pipe_inner_box", // name
					 aper1In + lengthSafety,     // x half width
					 aper2In + lengthSafety,     // y half width
					 1.7*lengthIn); // z long for unambiguous intersection
  //beampipe inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* bpInnerSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpInnerCylSolid,              // solid 1
						   bpInnerRectSolid);            // solid 2

  //beampipe outer edge for subtraction (actually just like vacuum + lengthSafety)
  //this length should be less than bpInnerSolid above but longer than the actual length for later intersection
  G4VSolid* bpOuterCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid", // name
						   aper3In + beamPipeThicknessIn,    // horizontal semi-axis
						   aper4In + beamPipeThicknessIn,    // vertical semi-axis
						   lengthIn);                        // length
  //beampipe outer edge box solid (rectangular cross-section)
  G4VSolid* bpOuterRectSolid = new G4Box(nameIn + "_pipe_inner_box",    // name
					 aper1In + beamPipeThicknessIn, // x half width
					 aper2In + beamPipeThicknessIn, // y half width
					 1.1*lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* bpOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpOuterCylSolid,              // solid 1
						   bpOuterRectSolid);            // solid 2
  //correct solid shape but too long
  G4VSolid* longBeamPipeSolid = new G4SubtractionSolid(nameIn + "_long_pipe_solid",  // name
						       bpOuterSolid,            // this
						       bpInnerSolid);           // minus this
  //final beampipe solid with correct shape and angled faces
  beamPipeSolid = new G4IntersectionSolid(nameIn + "_pipe_solid", // name
					  longBeamPipeSolid,      // solid1
					  vacuumAngledSolid);     // solid2

  //container solid
  //container cylindrical solid (circular cross-section)
  G4VSolid* contCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid",                 // name
						aper3In + beamPipeThicknessIn + lengthSafety, // horizontal semi-axis
						aper4In + beamPipeThicknessIn + lengthSafety, // vertical semi-axis
						lengthIn*0.5);                                // half length
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				      aper1In + beamPipeThicknessIn + lengthSafety, // x half width
				      aper2In + beamPipeThicknessIn + lengthSafety, // y half width
				      lengthIn); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  G4VSolid* longContainerSolid = new G4IntersectionSolid(nameIn + "_long_container_solid", // name
							 contCylSolid,                     // solid 1
							 contRectSolid);                   // solid 2

  containerSolid = new G4IntersectionSolid(nameIn + "_container_solid", // name
					   longContainerSolid,          // solid 1
					   vacuumAngledSolid);          // solid 2
}

void BDSBeamPipeFactoryRectEllipse::CreateContainerSubtractionSolid(G4String& nameIn,
								    G4double& lengthIn,
								    G4double& beamPipeThicknessIn,
								    G4double& aper1In,
								    G4double& aper2In,
								    G4double& aper3In,
								    G4double& aper4In)
{
  //container cylindrical solid (circular cross-section)
  G4VSolid* contSubCylSolid = new G4EllipticalTube(nameIn + "_subtraction_ellipsoid", // name
						   aper3In + beamPipeThicknessIn + lengthSafety, // horizontal semi-axis
						   aper4In + beamPipeThicknessIn + lengthSafety, // vertical semi-axis
						   2*lengthIn);                  // long length for unambiguous subtraction
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
