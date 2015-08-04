#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryRectAboveGround.hh"

#include "BDSDebug.hh"
#include "BDSTunnelInfo.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <utility>                         // for std::pair

class BDSTunnelSection;

BDSTunnelFactoryRectAboveGround* BDSTunnelFactoryRectAboveGround::_instance = 0;

BDSTunnelFactoryRectAboveGround* BDSTunnelFactoryRectAboveGround::Instance()
{
  if (_instance == 0)
    {_instance = new BDSTunnelFactoryRectAboveGround();}
  return _instance;
}

BDSTunnelFactoryRectAboveGround::BDSTunnelFactoryRectAboveGround()
{
  slabSolid      = nullptr;
  slabYHalfWidth = 1.5*CLHEP::m;
}

BDSTunnelFactoryRectAboveGround::~BDSTunnelFactoryRectAboveGround()
{
  _instance = 0;
}

BDSTunnelSection* BDSTunnelFactoryRectAboveGround::CreateTunnelSection(G4String    name,
								       G4double    length,
								       G4double    tunnelThickness,
								       G4double    tunnelSoilThickness,
								       G4Material* tunnelMaterial,
								       G4Material* tunnelSoilMaterial,
								       G4bool      tunnelFloor,
								       G4double    tunnelFloorOffset,
								       G4double    tunnel1,
								       G4double    tunnel2,
								       G4bool      visible)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // tidy up things from previous usage if any - base class method
  TidyUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1, tunnel2);

  // build the solids
  // work out how wide the slab needs to be
  G4double slabXHalfWidth = std::max(tunnel1, tunnel2) * 2;
  slabSolid = new G4Box(name + "_slab_solid",       // name
			slabXHalfWidth,             // x half width
			slabYHalfWidth,             // y half width
			0.5*length - lengthSafety); // z half width
			
  
  G4VSolid* tunnelOuterSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + tunnelThickness,    // x radius
					 tunnel2 + tunnelThickness,    // y radius
					 0.5*length - lengthSafety);   // z half length (to fit in container)
  
  G4VSolid* tunnelInnerSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + lengthSafety,       // x radius
					 tunnel2 + lengthSafety,       // y radius
					 length); // z half length - long for unambiguous subtraction

  G4VSolid* tunnelRectSolid = new G4SubtractionSolid(name + "_tunnel_rect_solid",// name
						     tunnelOuterSolid,           // this
						     tunnelInnerSolid);          // minus this

  // register solids
  solidsToBeRegistered.push_back(tunnelOuterSolid);
  solidsToBeRegistered.push_back(tunnelInnerSolid);
  solidsToBeRegistered.push_back(tunnelRectSolid);

  G4double yDisp = tunnel2 + slabYHalfWidth + lengthSafety; // lengthSafety ensures clean union with floor plane
  G4ThreeVector slabDisplacement = G4ThreeVector(0,-yDisp,0);
  tunnelSolid = new G4UnionSolid(name + "_tunnel_solid", // name
				 slabSolid,
				 tunnelRectSolid,
				 0,
				 slabDisplacement);
  
  G4double containerXRadius = slabXHalfWidth + lengthSafety;
  G4double containerYRadius = slabYHalfWidth + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      G4double floorThickness = tunnel2 - tunnelFloorOffset - lengthSafety;

      // placement vector for floor - assinging default 0,0,0 in base class
      floorDisplacement = G4ThreeVector(0, -(tunnelFloorOffset + floorThickness*0.5), 0);
      floorSolid = new G4Box(name + "_floor_solid",      // name
			     tunnel1,                    // x half width
			     floorThickness*0.5,         // y half width
			     0.5*length - lengthSafety); // z half width
      
      // need to create a container for the tunnel + floor that only just contains it
      G4VSolid* tunnelContainerOuterSlab = new G4Box(name + "_cont_slab_solid",     // name
						     slabXHalfWidth + lengthSafety, // x half width
						     slabYHalfWidth + lengthSafety, // y half width
						     0.5*length);                   // z half width
      G4VSolid* tunnelContainerOuterTunnel = new G4Box(name + "_cont_tunnel_solid",              // name
						       tunnel1 + tunnelThickness + lengthSafety, // x radius
						       tunnel2 + tunnelThickness + lengthSafety, // y radius
						       0.5*length);                              // z half length
      G4VSolid* tunnelContainerOuter = new G4UnionSolid(name + "_cont_outer_solid",
							tunnelContainerOuterSlab,
							tunnelContainerOuterTunnel);

      G4double tunnelContInnerYRadius = ( tunnelFloorOffset + tunnel2 ) * 0.5;
      G4VSolid* tunnelContainerInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						 tunnel1,                           // x radius
						 tunnelContInnerYRadius,            // y radius
						 length*0.5);

      // regsiter solids
      solidsToBeRegistered.push_back(tunnelContainerOuterSlab);
      solidsToBeRegistered.push_back(tunnelContainerOuterTunnel);
      solidsToBeRegistered.push_back(tunnelContainerOuter);
      solidsToBeRegistered.push_back(tunnelContainerInner);

      // offset the centre cut out by the difference between the vertical half widths of tunnel2 and tunnel2+floor
      G4ThreeVector contInsideDisplacement = G4ThreeVector(0, tunnelContInnerYRadius - tunnel2, 0);
      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerOuter,        // this
					      tunnelContainerInner,        // minus this
					      0,                           // rotate by this
					      contInsideDisplacement);
    }
  else
    {
      // have to do a subtraction
      G4VSolid* tunnelContainerOuterSlab = new G4Box(name + "_cont_slab_solid",     // name
						     slabXHalfWidth + lengthSafety, // x half width
						     slabYHalfWidth + lengthSafety, // y half width
						     0.5*length);                   // z half width
      G4VSolid* tunnelContainerOuterTunnel = new G4Box(name + "_cont_tunnel_solid",              // name
						       tunnel1 + tunnelThickness + lengthSafety, // x radius
						       tunnel2 + tunnelThickness + lengthSafety, // y radius
						       0.5*length);                              // z half length
      G4VSolid* tunnelContainerOuter = new G4UnionSolid(name + "_cont_outer_solid",
							tunnelContainerOuterSlab,
							tunnelContainerOuterTunnel);
      
      G4VSolid* tunnelContainerInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						 tunnel1,                           // x radius
						 tunnel2,                           // y radius
						 length);

      // regsiter solids
      solidsToBeRegistered.push_back(tunnelContainerOuterSlab);
      solidsToBeRegistered.push_back(tunnelContainerOuterTunnel);
      solidsToBeRegistered.push_back(tunnelContainerOuter);
      solidsToBeRegistered.push_back(tunnelContainerInner);

      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerOuter,        // this
					      tunnelContainerInner);       // minus this
    }

  G4double radius = std::max(containerXRadius, containerYRadius);
  BuildReadOutVolumeStraight(name, length, radius);

  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerXRadius, containerYRadius, visible);
  
  return tunnelSection;
}

BDSTunnelSection* BDSTunnelFactoryRectAboveGround::CreateTunnelSectionAngled(G4String      name,
									     G4double      length,
									     G4ThreeVector inputFace,
									     G4ThreeVector outputFace,
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
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // tidy up things from previous usage if any - base class method
  TidyUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1, tunnel2);
  
  // build the solids
  // create an intersection cut tubs to get the faces - make it bigger than everything else
  // then make elliptical solids longer than they need to be
  G4double intersectionRadius = ( std::max(tunnel1,tunnel2) + tunnelThickness + tunnelSoilThickness ) * 3;
  G4VSolid* faceSolid = new G4CutTubs(name + "_face_intersection_solid", // name
				      0,                                 // inner radius
				      intersectionRadius,                // outer radius
				      length*0.5 - lengthSafety,         // z half length
				      0,                                 // start angle
				      CLHEP::twopi,                      // sweep angle
				      inputFace,                         // input face normal vector
				      outputFace);                       // output face normal vector


  // tunnel
  G4VSolid* tunnelOuterSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + tunnelThickness,    // x radius
					 tunnel2 + tunnelThickness,    // y radius
					 length);                      // z half length
  
  G4VSolid* tunnelInnerSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + lengthSafety,       // x radius
					 tunnel2 + lengthSafety,       // y radius
					 length*1.5); // z half length - long for unambiguous subtraction

  G4VSolid* tunnelSolidUnAngled = new G4SubtractionSolid(name + "_tunnel_square_solid", // name
							 tunnelOuterSolid,      // this
							 tunnelInnerSolid);     // minus this

  // register solids
  solidsToBeRegistered.push_back(tunnelOuterSolid);
  solidsToBeRegistered.push_back(tunnelInnerSolid);
  solidsToBeRegistered.push_back(tunnelSolidUnAngled);

  // cut off the faces with the angled face solid
  tunnelSolid = new G4IntersectionSolid(name + "_tunnel_solid", // name
					tunnelSolidUnAngled,
					faceSolid);

  // soil solid
  G4double soil1R = tunnel1 + tunnelThickness + lengthSafety;
  G4double soil2R = tunnel2 + tunnelThickness + lengthSafety;

  G4double soilOuterRadius = std::max(soil1R, soil2R) + tunnelSoilThickness;

  G4VSolid* soilOuterSolid = new G4Tubs(name + "_soil_outer_solid",   // name
					0,                            // inner radius
					soilOuterRadius,              // outer radius
					length,                       // z half angle
					0,                            // start angle
					CLHEP::twopi);                // sweep angle
  
  G4VSolid* soilInnerSolid = new G4Box(name + "_soil_outer_solid",   // name
				       soil1R + lengthSafety,        // x radius
				       soil2R + lengthSafety,        // y radius
				       1.5*length); // z half length - long for unambiguous subtraction
  
  G4VSolid* soilSolidUnAngled = new G4SubtractionSolid(name + "_soil_square_solid", // name
						       soilOuterSolid,      // this
						       soilInnerSolid);     // minus this

  // register solids
  solidsToBeRegistered.push_back(soilOuterSolid);
  solidsToBeRegistered.push_back(soilInnerSolid);
  solidsToBeRegistered.push_back(soilSolidUnAngled);
  
  soilSolid = new G4IntersectionSolid(name + "_soil_soild", // name
				      soilSolidUnAngled,
				      faceSolid);

  G4double containerXRadius = soil1R + tunnelSoilThickness + lengthSafety;
  G4double containerYRadius = soil2R + tunnelSoilThickness + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      // these three lines are a repeat of the same part in the first function (~L211)
      G4double floorThickness = tunnel2 - tunnelFloorOffset - lengthSafety;

      // placement vector for floor - assinging default 0,0,0 in base class
      floorDisplacement = G4ThreeVector(0, -(tunnelFloorOffset + floorThickness*0.5), 0);

      G4VSolid* floorSolidSquare = new G4Box(name + "_floor_box_solid",  // name
					     tunnel1,                    // x half width
					     floorThickness*0.5,         // y half width
					     length);                    // z half width
      
      floorSolid = new G4IntersectionSolid(name + "_floor_solid", // name
					   floorSolidSquare,
					   faceSolid);
      
      // need to create a container for the tunnel + floor that only just contains it
      G4VSolid* tunnelContainerSolidOuter = new G4CutTubs(name + "_tunnel_cont_solid_outer", // name
							  0,                                 // inner radius
							  soilOuterRadius + lengthSafety,    // outer radius
							  0.5*length,                        // z half angle
							  0,                                 // start angle
							  CLHEP::twopi,                      // sweep angle
							  inputFace,                         // input face normal vector
							  outputFace);                       // output face normal vector

      G4double tunnelContInnerYRadius = ( tunnelFloorOffset + tunnel2 ) * 0.5;
      G4VSolid* tunnelContainerSolidInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						      tunnel1,                           // x radius
						      tunnelContInnerYRadius,            // y radius
						      length);

      // register solids
      solidsToBeRegistered.push_back(floorSolidSquare);
      solidsToBeRegistered.push_back(tunnelContainerSolidOuter);
      solidsToBeRegistered.push_back(tunnelContainerSolidInner);

      // offset the centre cut out by the difference between the vertical half widths of tunnel2 and tunnel2+floor
      G4ThreeVector contInsideDisplacement = G4ThreeVector(0, tunnelContInnerYRadius - tunnel2, 0);
      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerSolidOuter,   // this
					      tunnelContainerSolidInner,   // minus this
					      0,                           // rotate by this
					      contInsideDisplacement);      
    }
  else
    {
      // have to do a subtraction
      G4VSolid* tunnelContainerSolidOuter = new G4CutTubs(name + "_tunnel_cont_solid_outer",     // name
							  0,                                     // inner radius
							  soilOuterRadius + lengthSafety,        // outer radius
							  0.5*length,                            // z half angle
							  0,                                     // start angle
							  CLHEP::twopi,                          // sweep angle
							  inputFace,                             // input normal vector
							  outputFace);                           // output normal vector
      
      G4VSolid* tunnelContainerSolidInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						      tunnel1,                           // x radius
						      tunnel2,                           // y radius
						      length);

      // register solids
      solidsToBeRegistered.push_back(tunnelContainerSolidOuter);
      solidsToBeRegistered.push_back(tunnelContainerSolidInner);

      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerSolidOuter,   // this
					      tunnelContainerSolidInner);  // minus this
    } 

  G4double radius = std::max(containerXRadius, containerYRadius);
  BuildReadOutVolumeAngled(name, length, radius, inputFace, outputFace);
  
  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerXRadius, containerYRadius, visible);
  
  return tunnelSection;
}

/// functions below here are private to this particular factory
void BDSTunnelFactoryRectAboveGround::TestInputParameters(G4double&    length,
							  G4double&    tunnelThickness,
							  G4double&    tunnelSoilThickness,
							  G4Material*& tunnelMaterial,
							  G4Material*& tunnelSoilMaterial,
							  G4double&    tunnelFloorOffset,
							  G4double&    tunnel1,
							  G4double&    tunnel2)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);
  
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel->floorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel->aper1;}

  if (tunnel2 < 1e-10)
    {tunnel2 = defaultModel->aper2;}
}
