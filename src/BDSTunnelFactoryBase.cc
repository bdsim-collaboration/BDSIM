#include "BDSTunnelFactoryBase.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"          // for vis debug flag
#include "BDSMaterials.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"            // for calculateorientation

#include "globals.hh"                 // geant4 globals / types
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"


BDSTunnelFactoryBase::BDSTunnelFactoryBase():
  tunnelSection(NULL),
  containerSolid(NULL), tunnelSolid(NULL), soilSolid(NULL), floorSolid(NULL),
  containerLV(NULL), tunnelLV(NULL), soilLV(NULL), floorLV(NULL),
  floorOffset(G4ThreeVector(0,0,0))
{
  lengthSafety   = BDSGlobalConstants::Instance()->GetLengthSafety();
}

BDSGeometryComponent* BDSTunnelFactoryBase::CreateTunnelSectionAngledIn(G4String      name,
									G4double      length,
									G4double      angleIn,
									G4double      tunnelThickness,
									G4double      tunnelSoilThickness,
									G4Material*   tunnelMaterial,
									G4Material*   tunnelSoilMaterial,
									G4bool        tunnelFloor,
									G4double      tunnelFloorOffset,
									G4double      tunnel1,
									G4double      tunnel2)
{
  return CreateTunnelSectionAngledInOut(name, length, angleIn, 0, tunnelThickness,
					tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
					tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2);
}

BDSGeometryComponent* BDSTunnelFactoryBase::CreateTunnelSectionAngledOut(G4String      name,
									 G4double      length,
									 G4double      angleOut,
									 G4double      tunnelThickness,
									 G4double      tunnelSoilThickness,
									 G4Material*   tunnelMaterial,
									 G4Material*   tunnelSoilMaterial,
									 G4bool        tunnelFloor,
									 G4double      tunnelFloorOffset,
									 G4double      tunnel1,
									 G4double      tunnel2)
{
  return CreateTunnelSectionAngledInOut(name, length, 0, angleOut, tunnelThickness,
					tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
					tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2);
}

std::pair<G4ThreeVector,G4ThreeVector> BDSTunnelFactoryBase::CalculateFaces(G4double angleIn,
									      G4double angleOut)
{
  /// orientation -1,0,1 value - always use |angle| with trigonometric and then
  /// multiply by this factor, 0 by default
  G4int orientationIn  = BDS::CalculateOrientation(angleIn);
  G4int orientationOut = BDS::CalculateOrientation(angleOut);
  
  G4double in_z  = cos(fabs(angleIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = sin(fabs(angleIn)); // note full angle here as it's the exit angle
  G4double out_z = cos(fabs(angleOut));
  G4double out_x = sin(fabs(angleOut));
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);   // no output face angle
  return std::make_pair(inputface,outputface);
}

void BDSTunnelFactoryBase::CommontTestInputParameters(G4double&    length,
						      G4double&    tunnelThickness,
						      G4double&    tunnelSoilThickness,
						      G4Material*& tunnelMaterial,
						      G4Material*& tunnelSoilMaterial)
{
  if (length < 4*lengthSafety)
    {
      G4cerr << __METHOD_NAME__ << "tunnel section too short - length < 4*length safety " << G4endl;
      exit(1);
    }
  if (tunnelThickness < 2*lengthSafety)
    {
      G4cerr << __METHOD_NAME__ << "tunnel section thickness too thin - thicknes < 2*length safety " << G4endl;
      exit(1);
    }
  if (tunnelSoilThickness < 2*lengthSafety)
    {
      G4cerr << __METHOD_NAME__ << "tunnel section soil thickness too thin - thicknes < 2*length safety "
	     << G4endl;
      exit(1);
    }
  if (!tunnelMaterial)
    {tunnelMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetTunnelMaterial());}

  if (!tunnelSoilMaterial)
    {tunnelSoilMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetTunnelSoilMaterial());}
}

void BDSTunnelFactoryBase::CommonConstruction(G4String    name,
					      G4Material* tunnelMaterial,
					      G4Material* tunnelSoilMaterial,
					      G4double    length)

{
  BuildLogicalVolumes(name, tunnelMaterial, tunnelSoilMaterial);
  SetVisAttributes();
  SetUserLimits(length);
  PlaceComponents(name);
  PrepareGeometryComponent();
  SetSensitiveVolumes();
}

void BDSTunnelFactoryBase::BuildLogicalVolumes(G4String   name,
					       G4Material* tunnelMaterial,
					       G4Material* tunnelSoilMaterial)
{  
  // build logical volumes
  G4Material* emptyMaterial    = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
  
  tunnelLV    = new G4LogicalVolume(tunnelSolid,
				    tunnelMaterial,
				    name + "_tunnel_lv");
  
  soilLV      = new G4LogicalVolume(soilSolid,
				    tunnelSoilMaterial,
				    name + "_soil_lv");

  // remember the floor is optional
  if (floorSolid)
    {
      floorLV     = new G4LogicalVolume(floorSolid,
					tunnelMaterial,
					name + "_floor_lv");
    }
}

void BDSTunnelFactoryBase::SetVisAttributes()
{ 
  // VISUAL ATTRIBUTES
  // tunnel & floor - a nice grey
  G4VisAttributes* tunnelVisAttr = new G4VisAttributes(G4Colour(0.545, 0.533, 0.470));
  tunnelVisAttr->SetVisibility(true);
  tunnelVisAttr->SetForceSolid(true);
  tunnelLV->SetVisAttributes(tunnelVisAttr);
  if (floorLV)
    {floorLV->SetVisAttributes(tunnelVisAttr);}
  // soil - brown
  G4VisAttributes* soilVisAttr = new G4VisAttributes(G4Colour(0.545, 0.353, 0));
  soilVisAttr->SetVisibility(true);
  soilVisAttr->SetForceSolid(true);
  soilLV->SetVisAttributes(soilVisAttr);
  // container
  if (BDSExecOptions::Instance()->GetVisDebug()) {
    containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
  } else {
    containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  }
}

void BDSTunnelFactoryBase::PrepareGeometryComponent()
{
  // prepare final object and register logical volumes
  tunnelSection = new BDSGeometryComponent(containerSolid, containerLV);
  tunnelSection->RegisterLogicalVolume(tunnelLV);
  tunnelSection->RegisterLogicalVolume(soilLV);
  if (floorLV)
    {tunnelSection->RegisterLogicalVolume(floorLV);}
}

void BDSTunnelFactoryBase::SetSensitiveVolumes()
{
  // SENSITIVITY
  // make the tunnel sensitive if required
  // uses read out geometry
  if (BDSGlobalConstants::Instance()->GetSensitiveTunnel())
    {
      // ******* TBC********* uncomment when we merge in magnetouterbranch into develop / this one
      //tunnelSection->RegisterSensitiveVolume(tunnelLV);
      //tunnelSection->RegisterSensitiveVolume(soilLV);
      //if (floorLV)
      //{tunnelSection->RegisterSensitiveVolume(floorLV);}
    }
}

void BDSTunnelFactoryBase::SetUserLimits(G4double length)
{
  // USER LIMITS
  // set user limits based on bdsim user specified parameters
  G4UserLimits* tunnelUserLimits = new G4UserLimits("tunnel_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  tunnelUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  tunnelUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  tunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  //attach cuts to volumes
  tunnelLV->SetUserLimits(tunnelUserLimits);
  soilLV->SetUserLimits(tunnelUserLimits);
  containerLV->SetUserLimits(tunnelUserLimits);
  if (floorLV)
    {floorLV->SetUserLimits(tunnelUserLimits);}
}

void BDSTunnelFactoryBase::PlaceComponents(G4String name)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    tunnelLV,                     // lv to be placed
		    name + "_tunnel_pv",        // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    soilLV,                       // lv to be placed
		    name + "_soil_pv",          // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  if (floorLV)
    {
      new G4PVPlacement((G4RotationMatrix*)0,     // no rotation
			floorOffset,              // position
			floorLV,                  // lv to be placed
			name + "_floor_pv",     // name
			containerLV,              // mother lv to be place in
			false,                    // no boolean operation
			0,                        // copy number
			BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
			);
    }
}

void BDSTunnelFactoryBase::TidyUp()
{
  tunnelSection  = NULL;
  containerSolid = NULL;
  tunnelSolid    = NULL;
  soilSolid      = NULL;
  floorSolid     = NULL;
  containerLV    = NULL;
  tunnelLV       = NULL;
  soilLV         = NULL;
  floorLV        = NULL;
  floorOffset    = G4ThreeVector(0,0,0);
}
