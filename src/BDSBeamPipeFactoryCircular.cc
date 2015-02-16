#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipe.hh"

#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4Material.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4CutTubs.hh"
#include "G4ThreeVector.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

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
  lengthSafety   = BDSGlobalConstants::Instance()->GetLengthSafety();
  vacuumSolid    = NULL;
  beamPipeSolid  = NULL;
  containerSolid = NULL;
  vacuumLV       = NULL;
  beamPipeLV     = NULL;
  containerLV    = NULL;
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
			     lengthIn*0.5,                  // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  beamPipeSolid = new G4Tubs(nameIn + "_pipe_solid",        // name
			     aper1In + lengthSafety,        // inner radius + length safety to avoid overlaps
			     aper1In + beamPipeThicknessIn, // outer radius
			     lengthIn*0.5,                  // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
			      0,                            // inner radius
			      containerRadius,              // outer radius
			      lengthIn*0.5,                 // half length
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

  G4double orientation = 0;
  if (angleInIn < 0)
    {orientation = -1;}
  else
    {orientation = 1;}
  G4double in_z = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(fabs(angleInIn)); // note full angle here as it's the entrance angle
  G4ThreeVector inputface  = G4ThreeVector(orientation*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
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

  G4double orientation = 0;
  if (angleOutIn < 0)
    {orientation = 1;}
  else
    {orientation = -1;}
  G4double out_z = cos(fabs(angleOutIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double out_x = sin(fabs(angleOutIn)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(0.0, 0.0, -1.0); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientation*out_x, 0.0, out_z);               // no output face angle
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

  G4double orientationIn  = 0;
  G4double orientationOut = 0;
  if (angleInIn < 0)
    {orientationIn = -1;}
  else
    {orientationIn = 1;}
  if (angleOutIn < 0)
    {orientationOut = -1;}
  else
    {orientationOut = 1;}
  G4double in_z  = cos(fabs(angleInIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = sin(fabs(angleInIn)); // note full angle here as it's the exit angle
  G4double out_z = cos(fabs(angleOutIn));
  G4double out_x = sin(fabs(angleOutIn));
  G4ThreeVector inputface  = G4ThreeVector(-orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
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
    {vacuumMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterialName());}

  if (beamPipeThicknessIn < 1e-10)
    {beamPipeThicknessIn = BDSGlobalConstants::Instance()->GetBeampipeThickness();}

  if (!beamPipeMaterialIn)
    {beamPipeMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetPipeMaterialName());}

  if (aper1In < 1e-10)
    {aper1In = BDSGlobalConstants::Instance()->GetBeampipeRadius();}
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

  // set visual attributes
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(G4Color(0.4,0.4,0.4));
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceSolid(true);
  beamPipeLV->SetVisAttributes(pipeVisAttr);

  vacuumLV->SetVisAttributes(new G4VisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr()));
#ifdef BDSDEBUG
  containerLV->SetVisAttributes(new G4VisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
#else
  containerLV->SetVisAttributes(new G4VisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr()));
#endif
  
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
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extX,extY,extZ,vacuumLV,true,containerRadiusIn);
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
				lengthIn*0.5,                  // half length
				0,                             // rotation start angle
				CLHEP::twopi,                  // rotation finish angle
				inputfaceIn,                   // input face normal
				outputfaceIn );                // output face normal
			              
  
  beamPipeSolid = new G4CutTubs(nameIn + "_pipe_solid",        // name
				aper1In + lengthSafety,        // inner radius + length safety to avoid overlaps
				aper1In + beamPipeThicknessIn, // outer radius
				lengthIn*0.5,                  // half length
				0,                             // rotation start angle
				CLHEP::twopi,                  // rotation finish angle
				inputfaceIn,                   // input face normal
				outputfaceIn );                // output face normal
  

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
				 0,                            // inner radius
				 aper1In + beamPipeThicknessIn + lengthSafety,  // outer radius
				 lengthIn*0.5,                 // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputfaceIn,                  // input face normal
				 outputfaceIn);                // rotation finish angle
}
