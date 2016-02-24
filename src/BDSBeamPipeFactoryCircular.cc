#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>               // for std::pair

BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::_instance = nullptr;

BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSBeamPipeFactoryCircular();}
  return _instance;
}

BDSBeamPipeFactoryCircular::BDSBeamPipeFactoryCircular()
{;}

BDSBeamPipeFactoryCircular::~BDSBeamPipeFactoryCircular()
{
  _instance = nullptr;
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
   // clean up after last usage
  CleanUp();

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

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
								   G4double    lengthIn,            // length [mm]
								   G4double    angleInIn,           // the normal angle of the input face
								   G4double    angleOutIn,          // the normal angle of the output face
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
   // clean up after last usage
  CleanUp();

  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleInIn, angleOutIn);
  G4ThreeVector inputface  = faces.first;
  G4ThreeVector outputface = faces.second;

  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, containerRadius);
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
  allSolids.push_back(vacuumSolid);
  allSolids.push_back(beamPipeSolid);
  // prepare a longer container subtraction solid
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4Tubs(nameIn + "_container_sub_solid",// name
					 0,                              // inner radius
					 containerRadiusIn,              // outer radius
					 lengthIn*4,                     // full length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle
  
  BDSBeamPipeFactoryBase::CommonConstruction(nameIn,
					     vacuumMaterialIn,
					     beamPipeMaterialIn,
					     lengthIn);


  
  // record extents
  std::pair<double,double> extX = std::make_pair(-containerRadiusIn,containerRadiusIn);
  std::pair<double,double> extY = std::make_pair(-containerRadiusIn,containerRadiusIn);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);

  return BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(extX,extY,extZ,
								 containerRadiusIn);

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
  
  // beampipesolid created as subtraction since direct G4CutTubs creation created scattering in sector bends. not really understood
  
  G4VSolid* inside = new G4CutTubs(nameIn + "_pipe_inner_solid", // name
				   0,                               // inner radius + length safety to avoid overlaps
				   aper1In + lengthSafety,          // outer radius
				   lengthIn,                        // half length
				   0,                               // rotation start angle
				   CLHEP::twopi,                    // rotation finish angle
				   inputfaceIn,                     // input face normal
				   outputfaceIn );
  G4VSolid* outer  = new G4CutTubs(nameIn + "_pipe_outer_solid",  // name
				   0,                               // inner radius + length safety to avoid overlaps
				   aper1In + beamPipeThicknessIn,   // outer radius
				   lengthIn*0.5 - lengthSafety,     // half length
				   0,                               // rotation start angle
				   CLHEP::twopi,                    // rotation finish angle
				   inputfaceIn,                     // input face normal
				   outputfaceIn);
  allSolids.push_back(inside);
  allSolids.push_back(outer);
  
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",
					 outer,
					 inside);

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
				 0,                            // inner radius
				 aper1In + beamPipeThicknessIn + lengthSafety,  // outer radius
				 lengthIn*0.5-lengthSafety,    // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputfaceIn,                  // input face normal
				 outputfaceIn);                // rotation finish angle
}
