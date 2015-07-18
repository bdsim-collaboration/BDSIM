#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryRectangular.hh"

#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
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
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <utility>                         // for std::pair


BDSTunnelFactoryRectangular* BDSTunnelFactoryRectangular::_instance = 0;

BDSTunnelFactoryRectangular* BDSTunnelFactoryRectangular::Instance()
{
  if (_instance == 0)
    {_instance = new BDSTunnelFactoryRectangular();}
  return _instance;
}

BDSTunnelFactoryRectangular::BDSTunnelFactoryRectangular():BDSTunnelFactoryBase()
{
}

BDSTunnelFactoryRectangular::~BDSTunnelFactoryRectangular()
{
  _instance = 0;
}

BDSGeometryComponent* BDSTunnelFactoryRectangular::CreateTunnelSection(G4String      name,
								       G4double      length,
								       G4double      tunnelThickness,
								       G4double      tunnelSoilThickness,
								       G4Material*   tunnelMaterial,
								       G4Material*   tunnelSoilMaterial,
								       G4bool        tunnelFloor,
								       G4double      tunnelFloorOffset,
								       G4double      tunnel1,
								       G4double      tunnel2)
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
  G4VSolid* tunnelOuterSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + tunnelThickness,    // x radius
					 tunnel2 + tunnelThickness,    // y radius
					 0.5*length - lengthSafety);   // z half length (to fit in container)
  
  G4VSolid* tunnelInnerSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + lengthSafety,       // x radius
					 tunnel2 + lengthSafety,       // y radius
					 length); // z half length - long for unambiguous subtraction

  tunnelSolid = new G4SubtractionSolid(name + "_tunnel_solid", // name
				       tunnelOuterSolid,      // this
				       tunnelInnerSolid);     // minus this


  G4double soil1R = tunnel1 + tunnelThickness + lengthSafety;
  G4double soil2R = tunnel2 + tunnelThickness + lengthSafety;

  G4double soilOuterRadius = std::max(soil1R, soil2R) + tunnelSoilThickness;
  
  G4VSolid* soilOuterSolid = new G4Tubs(name + "_soil_outer_solid",   // name
					0,                            // inner radius
					soilOuterRadius,              // outer radius
					0.5*length - lengthSafety,    // z half angle
					0,                            // start angle
					CLHEP::twopi);                // sweep angle
  
  G4VSolid* soilInnerSolid = new G4Box(name + "_soil_outer_solid",   // name
				       soil1R + lengthSafety,        // x radius
				       soil2R + lengthSafety,        // y radius
				       length); // z half length - long for unambiguous subtraction

  soilSolid = new G4SubtractionSolid(name + "_soil_solid", // name
				     soilOuterSolid,      // this
				     soilInnerSolid);     // minus this
  
  G4double containerXRadius = soil1R + tunnelSoilThickness + lengthSafety;
  G4double containerYRadius = soil2R + tunnelSoilThickness + lengthSafety;
  
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
      G4VSolid* tunnelContainerSolidOuter = new G4Tubs(name + "_tunnel_cont_solid_outer",     // name
						       0,                                     // inner radius
						       soilOuterRadius + lengthSafety,        // outer radius
						       0.5*length,                            // z half angle
						       0,                                     // start angle
						       CLHEP::twopi);                         // sweep angle

      G4double tunnelContInnerYRadius = ( tunnelFloorOffset + tunnel2 ) * 0.5;
      G4VSolid* tunnelContainerSolidInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						      tunnel1,                           // x radius
						      tunnelContInnerYRadius,            // y radius
						      length*0.5);

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
      G4VSolid* tunnelContainerSolidOuter = new G4Tubs(name + "_tunnel_cont_solid_outer",     // name
						       0,                                     // inner radius
						       soilOuterRadius + lengthSafety,        // outer radius
						       length*0.5,                            // z half angle
						       0,                                     // start angle
						       CLHEP::twopi);                         // sweep angle
      
      G4VSolid* tunnelContainerSolidInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						      tunnel1,                           // x radius
						      tunnel2,                           // y radius
						      length);

      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerSolidOuter,   // this
					      tunnelContainerSolidInner);  // minus this
    } 

  CommonFinalConstruction(name, length, tunnelMaterial, tunnelSoilMaterial, containerXRadius, containerYRadius);
  
  return tunnelSection; // member variable geometry component that's assembled in base class
}


BDSGeometryComponent* BDSTunnelFactoryRectangular::CreateTunnelSectionAngledInOut(G4String      name,
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
										 G4double      tunnel2)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // tidy up things from previous usage if any - base class method
  TidyUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1, tunnel2);

  std::pair<G4ThreeVector,G4ThreeVector> faces = CalculateFaces(angleIn, angleOut);
  G4ThreeVector inputface  = faces.first;
  G4ThreeVector outputface = faces.second;
  
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
				      inputface,                         // input face normal vector
				      outputface);                       // output face normal vector


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
							  inputface,                         // input face normal vector
							  outputface);                       // output face normal vector

      G4double tunnelContInnerYRadius = ( tunnelFloorOffset + tunnel2 ) * 0.5;
      G4VSolid* tunnelContainerSolidInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						      tunnel1,                           // x radius
						      tunnelContInnerYRadius,            // y radius
						      length);

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
							  inputface,                             // input normal vector
							  outputface);                           // output normal vector
      
      G4VSolid* tunnelContainerSolidInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
						      tunnel1,                           // x radius
						      tunnel2,                           // y radius
						      length);

      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerSolidOuter,   // this
					      tunnelContainerSolidInner);  // minus this
    } 

  CommonFinalConstruction(name, length, tunnelMaterial, tunnelSoilMaterial, containerXRadius, containerYRadius);

  return tunnelSection;
}

/// functions below here are private to this particular factory
void BDSTunnelFactoryRectangular::TestInputParameters(G4double&    length,
						     G4double&    tunnelThickness,
						     G4double&    tunnelSoilThickness,
						     G4Material*& tunnelMaterial,
						     G4Material*& tunnelSoilMaterial,
						     G4double&    tunnelFloorOffset,
						     G4double&    tunnel1,
						     G4double&    tunnel2)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);

  BDSTunnelInfo* defaultModel = BDSGlobalConstants::Instance()->GetTunnelInfo();
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel->floorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel->aper1;}

  if (tunnel2 < 1e-10)
    {tunnel2 = defaultModel->aper2;}
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSGeometryComponent* BDSTunnelFactoryRectangular::CommonFinalConstruction(G4String    name,
									  G4double    length,
									  G4Material* tunnelMaterial,
									  G4Material* tunnelSoilMaterial,
									  G4double    containerXRadius,
									  G4double    containerYRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSTunnelFactoryBase::CommonConstruction(name,
					   tunnelMaterial,
					   tunnelSoilMaterial,
					   length);

  // record extents
  std::pair<double,double> extX = std::make_pair(-containerXRadius, containerXRadius);
  std::pair<double,double> extY = std::make_pair(-containerYRadius, containerYRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  BDSGeometryComponent* aTunnelSegment = new BDSGeometryComponent(containerSolid,
								  containerLV,
								  extX,
								  extY,
								  extZ);

  return aTunnelSegment;
}
