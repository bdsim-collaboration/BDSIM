#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelSection.hh"
#include "BDSUtilities.hh"            // for calculateorientation

#include "globals.hh"                 // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"

BDSTunnelFactoryBase::BDSTunnelFactoryBase():
  tunnelComponent(nullptr),tunnelSection(nullptr),
  containerSolid(nullptr), tunnelSolid(nullptr), soilSolid(nullptr), floorSolid(nullptr),
  intersectionSolid(nullptr), readOutSolid(nullptr),
  containerLV(nullptr), tunnelLV(nullptr), soilLV(nullptr), floorLV(nullptr),
  readOutLV(nullptr), floorDisplacement(G4ThreeVector(0,0,0)),
  cumulativeAngle(0), readOutRadius(0)
{
  // use large length safety for tunnel construction to avoid stuck particles
  // will not make difference to tracking so is acceptable to have 1um gap.
  lengthSafety  = 1*CLHEP::um;
  checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
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
  auto faces = BDS::CalculateFaces(angleIn,angleOut);
  return CreateTunnelSectionAngled(name, length, faces.first, faces.second, tunnelThickness,
				   tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
				   tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
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

void BDSTunnelFactoryBase::CommonConstruction(G4String      name,
					      G4Material*   tunnelMaterial,
					      G4Material*   tunnelSoilMaterial,
					      G4double      length,
					      G4double      containerXRadius,
					      G4double      containerYRadius,
					      G4bool        visible,
					      G4ThreeVector inputFace,
					      G4ThreeVector outputFace)

{
  readOutRadius = std::max(containerXRadius, containerYRadius);
  BuildReadOutVolumeAngled(name, length, readOutRadius, inputFace, outputFace);
  BuildLogicalVolumes(name, tunnelMaterial, tunnelSoilMaterial);
  SetVisAttributes(visible);
  SetUserLimits(length);
  PlaceComponents(name);
  PrepareGeometryComponent(containerXRadius, containerYRadius, 0.5*length);
  SetSensitiveVolumes();
  PrepareTunnelSection(name, length);
}


void BDSTunnelFactoryBase::CommonConstruction(G4String    name,
					      G4Material* tunnelMaterial,
					      G4Material* tunnelSoilMaterial,
					      G4double    length,
					      G4double    containerXRadius,
					      G4double    containerYRadius,
					      G4bool      visible)

{
  readOutRadius = std::max(containerXRadius, containerYRadius);
  BuildReadOutVolumeStraight(name, length, readOutRadius);
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
  G4Material* emptyMaterial    = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
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
  readOutLV = new G4LogicalVolume(readOutSolid,
				  emptyMaterial,
				  name + "_readout_lv");
}

void BDSTunnelFactoryBase::SetVisAttributes(G4bool visible)
{ 
  // note these could actually be owned by the factory or kept in one place as they're
  // always the same - this would however mean that the objects would have a dependency
  // on the factory which shouldn't happen - so waste a little memory just now on visattrs
  
  // tunnel & floor - a nice grey
  G4VisAttributes* tunnelVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("tunnel"));
  if (visible)
    {tunnelVisAttr->SetVisibility(true);}
  tunnelVisAttr->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle()*2);
  tunnelLV->SetVisAttributes(tunnelVisAttr);
  visAttributesToBeRegistered.push_back(tunnelVisAttr);
  if (floorLV)
    {
      G4VisAttributes* floorVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("tunnelfloor"));
      if (visible)
	{floorVisAttr->SetVisibility(true);}
      floorVisAttr->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle()*2);
      floorLV->SetVisAttributes(floorVisAttr);
      visAttributesToBeRegistered.push_back(floorVisAttr);
    }
  // soil - brown
  if (soilLV)
    {
      G4VisAttributes* soilVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("soil"));
      if (visible)
	{soilVisAttr->SetVisibility(true);}
      soilVisAttr->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
      soilLV->SetVisAttributes(soilVisAttr);
      visAttributesToBeRegistered.push_back(soilVisAttr);
    }
  // container & read out
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
  readOutLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
}

void BDSTunnelFactoryBase::PrepareGeometryComponent(G4double /*containerXRadius*/,
						    G4double /*containerYRadius*/,
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
  // use the read out geometry for the limits as it's the maximum of x and y
  // in radius. The read out world is a duplicate of the real mass world so
  // we fiddle the extents here as the extents of the read out don't exist and
  // this is relatively safe as nothing will be placed against the outside edge
  // of the soil.
  tunnelComponent->SetExtentX(std::make_pair(-readOutRadius, readOutRadius));
  tunnelComponent->SetExtentY(std::make_pair(-readOutRadius, readOutRadius));
  tunnelComponent->SetExtentZ(std::make_pair(-containerZRadius, containerZRadius));
}

void BDSTunnelFactoryBase::PrepareTunnelSection(G4String name,
						G4double chordLength)
{
  tunnelSection = new BDSTunnelSection(name,
				       chordLength,
				       cumulativeAngle,
				       tunnelComponent,
				       intersectionSolid,
				       readOutLV);
}
  

void BDSTunnelFactoryBase::SetSensitiveVolumes()
{
  // Register components as sensitive with BDSGeometryComponent instance.
  // As they use a (tunnel specific) read out geometry, the require attaching
  // only once the beamline has been constructed - done in BDSDetectorConstruction.
  // World must be already constructed, which requires beam line to be already constructed.
  tunnelComponent->RegisterSensitiveVolume(readOutLV);
  tunnelComponent->RegisterSensitiveVolume(tunnelLV);
  if (soilLV)
    {tunnelComponent->RegisterSensitiveVolume(soilLV);}
  if (floorLV)
    {tunnelComponent->RegisterSensitiveVolume(floorLV);}
}

void BDSTunnelFactoryBase::SetUserLimits(G4double length)
{
  // USER LIMITS
  // set user limits based on bdsim user specified parameters
  G4UserLimits* tunnelUserLimits = new G4UserLimits("tunnel_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  tunnelUserLimits->SetMaxAllowedStep(length * maxStepFactor);
  tunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
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
  tunnelSection     = nullptr;
  containerSolid    = nullptr;
  tunnelSolid       = nullptr;
  soilSolid         = nullptr;
  floorSolid        = nullptr;
  intersectionSolid = nullptr;
  readOutSolid      = nullptr;
  containerLV       = nullptr;
  tunnelLV          = nullptr;
  soilLV            = nullptr;
  floorLV           = nullptr;
  readOutLV         = nullptr;
  floorDisplacement = G4ThreeVector(0,0,0);
  cumulativeAngle   = 0;
  readOutRadius     = 0;
  solidsToBeRegistered.clear();
  visAttributesToBeRegistered.clear();
  userLimitsToBeRegistered.clear();
}

void BDSTunnelFactoryBase::BuildReadOutVolumeStraight(G4String name,
						      G4double length,
						      G4double radius)
{
  readOutSolid = new G4Tubs(name + "_readout_solid", // name
			    0,                       // inner radius
			    radius,                  // outer radius
			    0.5*length,              // z half length
			    0,                       // starting angle
			    CLHEP::twopi);           // sweep angle
}

void BDSTunnelFactoryBase::BuildReadOutVolumeAngled(G4String      name,
						    G4double      length,
						    G4double      radius,
						    G4ThreeVector inputFace,
						    G4ThreeVector outputFace)
{
  readOutSolid = new G4CutTubs(name + "_readout_solid", // name
			       0,                       // inner radius
			       radius,                  // outer radius
			       0.5*length,              // z half length
			       0,                       // starting angle
			       CLHEP::twopi,            // sweep angle
			       inputFace,               // input face normal vector
			       outputFace);             // output face normal vector
}
