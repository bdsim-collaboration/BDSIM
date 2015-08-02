#include "BDSTunnelFactoryBase.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"          // for vis debug flag
#include "BDSMaterials.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTunnelSection.hh"
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
  tunnelComponent(NULL),tunnelSection(NULL),
  containerSolid(NULL), tunnelSolid(NULL), soilSolid(NULL), floorSolid(NULL),
  intersectionSolid(NULL),
  containerLV(NULL), tunnelLV(NULL), soilLV(NULL), floorLV(NULL),
  floorDisplacement(G4ThreeVector(0,0,0)),
  cumulativeAngle(0)
{
  lengthSafety  = BDSGlobalConstants::Instance()->GetLengthSafety();
  checkOverlaps = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  defaultModel  = BDSGlobalConstants::Instance()->TunnelInfo();
}

BDSTunnelSection* BDSTunnelFactoryBase::CreateTunnelSectionAngledIn(G4String      name,
								    G4double      length,
								    G4double      angleIn,
								    G4double      tunnelThickness,
								    G4double      tunnelSoilThickness,
								    G4Material*   tunnelMaterial,
								    G4Material*   tunnelSoilMaterial,
								    G4bool        tunnelFloor,
								    G4double      tunnelFloorOffset,
								    G4double      tunnel1,
								    G4double      tunnel2,
								    G4bool        visible)
{
  return CreateTunnelSectionAngledInOut(name, length, angleIn, 0, tunnelThickness,
					tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
					tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactoryBase::CreateTunnelSectionAngledOut(G4String      name,
								     G4double      length,
								     G4double      angleOut,
								     G4double      tunnelThickness,
								     G4double      tunnelSoilThickness,
								     G4Material*   tunnelMaterial,
								     G4Material*   tunnelSoilMaterial,
								     G4bool        tunnelFloor,
								     G4double      tunnelFloorOffset,
								     G4double      tunnel1,
								     G4double      tunnel2,
								     G4bool        visible)
{
  return CreateTunnelSectionAngledInOut(name, length, 0, angleOut, tunnelThickness,
					tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
					tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactoryBase::CreateTunnelSectionAngledInOut(G4String      name,
								       G4double      length,
								       G4double      angleIn,
								       G4double      angleOut,
								       G4double      tunnelThickness,
								       G4double      tunnelSoilThickness,
								       G4Material*   tunnelMaterial,
								       G4Material*   tunnelSoilMaterial,
								       G4bool        tunnelFloor,
								       G4double      tunnelFloorOffset,
								       G4double      tunnel1,
								       G4double      tunnel2,
								       G4bool        visible)
{
  auto faces = CalculateFaces(angleIn,angleOut);
  return CreateTunnelSectionAngled(name, length, faces.first, faces.second, tunnelThickness,
				   tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
				   tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
}

std::pair<G4ThreeVector, G4ThreeVector> BDSTunnelFactoryBase::CalculateFaces(G4double angleIn,
									     G4double angleOut)
{
  /// set cumulative angle
  cumulativeAngle = angleIn + angleOut;
  
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
  return std::make_pair(inputface, outputface);
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
      G4cerr << __METHOD_NAME__ << "tunnel section thickness too thin - thickness < 2*length safety " << G4endl;
      exit(1);
    }
  if (tunnelSoilThickness < 2*lengthSafety)
    {
      G4cerr << __METHOD_NAME__ << "tunnel section soil thickness too thin - thickness < 2*length safety "
	     << G4endl;
      exit(1);
    }
  if (!tunnelMaterial)
    {tunnelMaterial = defaultModel->material;}

  if (!tunnelSoilMaterial)
    {tunnelSoilMaterial = defaultModel->soilMaterial;}
}

void BDSTunnelFactoryBase::CommonConstruction(G4String    name,
					      G4Material* tunnelMaterial,
					      G4Material* tunnelSoilMaterial,
					      G4double    length,
					      G4double    containerXRadius,
					      G4double    containerYRadius,
					      G4bool      visible)

{
  BuildLogicalVolumes(name, tunnelMaterial, tunnelSoilMaterial);
  SetVisAttributes(visible);
  SetUserLimits(length);
  PlaceComponents(name);
  PrepareGeometryComponent(containerXRadius, containerYRadius, 0.5*length);
  SetSensitiveVolumes();
  PrepareTunnelSection(name, length);
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

  if (soilSolid)
    {
      soilLV      = new G4LogicalVolume(soilSolid,
					tunnelSoilMaterial,
					name + "_soil_lv");
    }
  // remember the floor is optional
  if (floorSolid)
    {
      floorLV     = new G4LogicalVolume(floorSolid,
					tunnelMaterial,
					name + "_floor_lv");
    }
}

void BDSTunnelFactoryBase::SetVisAttributes(G4bool visible)
{ 
  // note these could actually be owned by the factory or kept in one place as they're
  // always the same - this would however mean that the objects would have a dependency
  // on the factory which shouldn't happen - so waste a little memory just now on visattrs
  
  // tunnel & floor - a nice grey
  G4VisAttributes* tunnelVisAttr = new G4VisAttributes(G4Colour(0.545, 0.533, 0.470));
  if (visible)
    {tunnelVisAttr->SetVisibility(true);}
  tunnelVisAttr->SetForceLineSegmentsPerCircle(100);
  tunnelLV->SetVisAttributes(tunnelVisAttr);
  visAttributesToBeRegistered.push_back(tunnelVisAttr);
  if (floorLV)
    {
      G4VisAttributes* floorVisAttr = new G4VisAttributes(G4Colour(0.5, 0.5, 0.45));
      if (visible)
	{floorVisAttr->SetVisibility(true);}
      floorVisAttr->SetForceLineSegmentsPerCircle(100);
      floorLV->SetVisAttributes(floorVisAttr);
      visAttributesToBeRegistered.push_back(floorVisAttr);
    }
  // soil - brown
  if (soilLV)
    {
      G4VisAttributes* soilVisAttr = new G4VisAttributes(G4Colour(0.545, 0.353, 0, 0.4));
      if (visible)
	{soilVisAttr->SetVisibility(true);}
      soilVisAttr->SetForceLineSegmentsPerCircle(50);
      soilLV->SetVisAttributes(soilVisAttr);
      visAttributesToBeRegistered.push_back(soilVisAttr);
    }
  // container
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}
}

void BDSTunnelFactoryBase::PrepareGeometryComponent(G4double containerXRadius,
						    G4double containerYRadius,
						    G4double containerZRadius)
{
  // prepare final object and register logical volumes
  tunnelComponent = new BDSGeometryComponent(containerSolid, containerLV);
  tunnelComponent->RegisterLogicalVolume(tunnelLV);
  tunnelComponent->RegisterLogicalVolume(soilLV);
  if (floorLV)
    {tunnelComponent->RegisterLogicalVolume(floorLV);}

  // register objects
  tunnelComponent->RegisterSolid(solidsToBeRegistered);
  tunnelComponent->RegisterVisAttributes(visAttributesToBeRegistered);
  tunnelComponent->RegisterUserLimits(userLimitsToBeRegistered);

  // record extents
  tunnelComponent->SetExtentX(std::make_pair(-containerXRadius, containerXRadius));
  tunnelComponent->SetExtentY(std::make_pair(-containerYRadius, containerYRadius));
  tunnelComponent->SetExtentZ(std::make_pair(-containerZRadius, containerZRadius));
}

void BDSTunnelFactoryBase::PrepareTunnelSection(G4String name,
						G4double chordLength)
{
  tunnelSection = new BDSTunnelSection(name,
				       chordLength,
				       cumulativeAngle,
				       tunnelComponent,
				       intersectionSolid);
}
  

void BDSTunnelFactoryBase::SetSensitiveVolumes()
{
  // SENSITIVITY
  // make the tunnel sensitive if required
  // uses read out geometry
  /*
  if (BDSGlobalConstants::Instance()->GetSensitiveTunnel())
    {
      // ******* TBC********* uncomment when we merge in magnetouterbranch into develop / this one
      //tunnelSection->RegisterSensitiveVolume(tunnelLV);
      //tunnelSection->RegisterSensitiveVolume(soilLV);
      //if (floorLV)
      //{tunnelSection->RegisterSensitiveVolume(floorLV);}
      }*/
}

void BDSTunnelFactoryBase::SetUserLimits(G4double length)
{
  // USER LIMITS
  // set user limits based on bdsim user specified parameters
  G4UserLimits* tunnelUserLimits = new G4UserLimits("tunnel_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  tunnelUserLimits->SetMaxAllowedStep(length * maxStepFactor);
  tunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  //attach cuts to volumes
  tunnelLV->SetUserLimits(tunnelUserLimits);
  if (soilLV)
    {soilLV->SetUserLimits(tunnelUserLimits);}
  if (floorLV)
    {floorLV->SetUserLimits(tunnelUserLimits);}
  containerLV->SetUserLimits(tunnelUserLimits);

  // store for registration in finished component
  userLimitsToBeRegistered.push_back(tunnelUserLimits);
}

void BDSTunnelFactoryBase::PlaceComponents(G4String name)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  new G4PVPlacement((G4RotationMatrix*)0,     // no rotation
		    (G4ThreeVector)0,         // position
		    tunnelLV,                 // lv to be placed
		    name + "_tunnel_pv",      // name
		    containerLV,              // mother lv to be place in
		    false,                    // no boolean operation
		    0,                        // copy number
		    checkOverlaps);           // whether to check overlaps
  if (soilLV)
    {
      new G4PVPlacement((G4RotationMatrix*)0, // no rotation
			(G4ThreeVector)0,     // position
			soilLV,               // lv to be placed
			name + "_soil_pv",    // name
			containerLV,          // mother lv to be place in
			false,                // no boolean operation
			0,                    // copy number
		        checkOverlaps);       // whether to check overlaps
    }
  if (floorLV)
    {
      new G4PVPlacement((G4RotationMatrix*)0, // no rotation
			floorDisplacement,    // position
			floorLV,              // lv to be placed
			name + "_floor_pv",   // name
			containerLV,          // mother lv to be place in
			false,                // no boolean operation
			0,                    // copy number
		        checkOverlaps);       // whether to check overlaps
    }
}

void BDSTunnelFactoryBase::TidyUp()
{
  tunnelSection     = NULL;
  containerSolid    = NULL;
  tunnelSolid       = NULL;
  soilSolid         = NULL;
  floorSolid        = NULL;
  intersectionSolid = NULL;
  containerLV       = NULL;
  tunnelLV          = NULL;
  soilLV            = NULL;
  floorLV           = NULL;
  floorDisplacement = G4ThreeVector(0,0,0);
  cumulativeAngle   = 0;
  solidsToBeRegistered.clear();
  visAttributesToBeRegistered.clear();
  userLimitsToBeRegistered.clear();
}
