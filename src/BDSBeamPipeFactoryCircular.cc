#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipe.hh"

#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4Material.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

#include <utility>                         // for std::pair


BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::_instance = 0;

BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactoryCircular();}
  return _instance;
}

BDSBeamPipeFactoryCircular::BDSBeamPipeFactoryCircular(){;}

BDSBeamPipeFactoryCircular::~BDSBeamPipeFactoryCircular()
{
  _instance = 0;
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipe(G4String    nameIn,              // name
							G4double    lengthIn,            // length [mm]
							G4Material* vacuumMaterialIn,    // vacuum material
							G4double    beamPipeThicknessIn, // beampipe thickness [mm]
							G4Material* beamPipeMaterialIn,  // beampipe material
							G4double    aper1In,             // aperture parameter 1
							G4double    /*aper2In*/,         // aperture parameter 2
							G4double    /*aper3In*/,         // aperture parameter 3
							G4double    /*aper4In */         // aperture parameter 4
							)
{
  // test input parameters - set global options as default if not specified
  if (!vacuumMaterialIn)
    {vacuumMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterialName());}

  if (beamPipeThicknessIn < 1e-10)
    {beamPipeThicknessIn = BDSGlobalConstants::Instance()->GetBeampipeThickness();}

  if (!beamPipeMaterialIn)
    {beamPipeMaterialIn = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetPipeMaterialName());}

  if (aper1In < 1e-10)
    {aper1In = BDSGlobalConstants::Instance()->GetBeampipeRadius();}

  // build the solids
  G4double lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();
  
  G4VSolid* vacuumSolid   = new G4Tubs(nameIn + "_vacuum_solid",      // name
				       0,                             // inner radius
				       aper1In,                       // outer radius
				       lengthIn*0.5,                  // half length
				       0,                             // rotation start angle
				       CLHEP::twopi);                 // rotation finish angle
  
  G4VSolid* beampipeSolid = new G4Tubs(nameIn + "_pipe_solid",        // name
				       aper1In + lengthSafety,        // inner radius + length safety to avoid overlaps
				       aper1In + beamPipeThicknessIn, // outer radius
				       lengthIn*0.5,                  // half length
				       0,                             // rotation start angle
				       CLHEP::twopi);                 // rotation finish angle

  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  G4VSolid* containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
					0,                            // inner radius
					containerRadius,              // outer radius
					lengthIn*0.5,                 // half length
					0,                            // rotation start angle
					CLHEP::twopi);                // rotation finish angle
  
  // build the logical volumes
  G4LogicalVolume* vacuumLV   = new G4LogicalVolume(vacuumSolid,
						    vacuumMaterialIn,
						    nameIn + "_vacuum_lv");
  
  G4LogicalVolume* beampipeLV = new G4LogicalVolume(beampipeSolid,
						    beamPipeMaterialIn,
						    nameIn + "_beampipe_lv");

  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
						     vacuumMaterialIn,
						     nameIn + "_container_lv");

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
		    beampipeLV,                   // lv to be placed
		    nameIn + "_beampipe_pv",      // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  // record extents
  std::pair<double,double> extX = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);
  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extX,extY,extZ,vacuumLV,true,containerRadius);
  return aPipe;
}
 
BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledIn(G4String    nameIn,              // name
								G4double    lengthIn,            // length [mm]
								G4double    /*angleInIn*/,           // the normal angle of the input face
								G4Material* vacuumMaterialIn,    // vacuum material
								G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								G4Material* beamPipeMaterialIn,  // beampipe material
								G4double    aper1In,             // aperture parameter 1
								G4double    /*aper2In*/,         // aperture parameter 2
								G4double    /*aper3In*/,         // aperture parameter 3
								G4double    /*aper4In */         // aperture parameter 4
								)
{
  return CreateBeamPipe(nameIn, lengthIn, vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn, aper1In);
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledOut(G4String    nameIn,              // name
								 G4double    lengthIn,            // length [mm]
								 G4double    /*angleOutIn*/,           // the normal angle of the output face
								 G4Material* vacuumMaterialIn,    // vacuum material
								 G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								 G4Material* beamPipeMaterialIn,  // beampipe material
								 G4double    aper1In,             // aperture parameter 1
								 G4double    /*aper2In*/,         // aperture parameter 2
								 G4double    /*aper3In*/,         // aperture parameter 3
								 G4double    /*aper4In */         // aperture parameter 4
								 )
{
  return CreateBeamPipe(nameIn, lengthIn, vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn, aper1In);
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
								   G4double    lengthIn,            // length [mm]
								   G4double    /*angleInIn*/,           // the normal angle of the input face
								   G4double    /*angleOutIn*/,           // the normal angle of the input face
								   G4Material* vacuumMaterialIn,    // vacuum material
								   G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								   G4Material* beamPipeMaterialIn,  // beampipe material
								   G4double    aper1In,             // aperture parameter 1
								   G4double    /*aper2In*/,         // aperture parameter 2
								   G4double    /*aper3In*/,         // aperture parameter 3
								   G4double    /*aper4In */         // aperture parameter 4
								   )
{
  return CreateBeamPipe(nameIn, lengthIn, vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn, aper1In);
}
