#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryLHC.hh"
#include "BDSBeamPipe.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSDManager.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
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

BDSBeamPipeFactoryLHC::BDSBeamPipeFactoryLHC():BDSBeamPipeFactoryBase()
{
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

  std::pair<G4ThreeVector,G4ThreeVector> faces = CalculateFaces(angleInIn, angleOutIn);
  G4ThreeVector inputface  = faces.first;
  G4ThreeVector outputface = faces.second;

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
  BDSBeamPipeFactoryBase::TestInputParameters(vacuumMaterialIn,beamPipeThicknessIn,beamPipeMaterialIn);

  if (aper1In < 1e-10)
    {aper1In = BDSGlobalConstants::Instance()->GetBeamPipeRadius();}

  if (aper2In < 1e-10)
    {aper2In = BDSGlobalConstants::Instance()->GetAper2();}

  if (aper3In < 1e-10)
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

  // register sensitive volumes
  aPipe->RegisterSensitiveVolume(beamPipeLV);
  aPipe->RegisterSensitiveVolume(containerLV);
  
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
