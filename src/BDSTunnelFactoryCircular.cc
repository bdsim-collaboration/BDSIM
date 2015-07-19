#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryCircular.hh"

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
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <utility>                         // for std::pair


BDSTunnelFactoryCircular* BDSTunnelFactoryCircular::_instance = 0;

BDSTunnelFactoryCircular* BDSTunnelFactoryCircular::Instance()
{
  if (_instance == 0)
    {_instance = new BDSTunnelFactoryCircular();}
  return _instance;
}

BDSTunnelFactoryCircular::BDSTunnelFactoryCircular():BDSTunnelFactoryBase()
{
}

BDSTunnelFactoryCircular::~BDSTunnelFactoryCircular()
{
  _instance = 0;
}

BDSGeometryComponent* BDSTunnelFactoryCircular::CreateTunnelSection(G4String      name,
								    G4double      length,
								    G4double      tunnelThickness,
								    G4double      tunnelSoilThickness,
								    G4Material*   tunnelMaterial,
								    G4Material*   tunnelSoilMaterial,
								    G4bool        tunnelFloor,
								    G4double      tunnelFloorOffset,
								    G4double      tunnel1,
								    G4double      /*tunnel2*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // tidy up things from previous usage if any - base class method
  TidyUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);

  // build the solids
  tunnelSolid = new G4Tubs(name + "_tunnel_solid",    // name
			   tunnel1 + lengthSafety,    // inner radius
			   tunnel1 + tunnelThickness, // outer radius
			   length*0.5 - lengthSafety, // z half length
			   0,                         // start angle
			   CLHEP::twopi);             // sweep angle

  G4double soilInnerR = tunnel1 + tunnelThickness + lengthSafety;
  G4double soilOuterR = soilInnerR + tunnelSoilThickness;
  soilSolid   = new G4Tubs(name + "_soil_solid",      // name
			   soilInnerR,                // inner radius
			   soilOuterR,                // outer radius
			   length*0.5 - lengthSafety, // z half length
			   0,                         // start angle
			   CLHEP::twopi);             // sweep angle 

  G4double containerRadius = soilOuterR + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      G4double floorBoxRadius = 1.5*tunnel1; // will definitely encompass the tunnel
      G4double floorCylinderRadius = tunnel1 - lengthSafety; // so it fits within the tunnel
      G4double floorBoxDisplacement = tunnelFloorOffset + floorBoxRadius + lengthSafety;
      
      G4VSolid* floorCylinder = new G4Tubs(name + "_floor_cylinder_solid", // name
					   0,                              // inner radius
					   floorCylinderRadius,            // outer radius
					   length*0.5 - lengthSafety,      // z half length
					   0,                              // start angle
					   CLHEP::twopi);                  // sweep angle

      G4VSolid* floorBox      = new G4Box(name + "_floor_box_solid",  // name
					  floorBoxRadius,             // x half width
					  floorBoxRadius,             // y half width
					  length);                    // z half length
      // box z length extra long for unambiguous subtraction
      
      floorSolid = new G4IntersectionSolid(name + "_floor_solid",                     // name
					   floorCylinder,                             // this
					   floorBox,                                  // minus this,
					   0,                                         // rotation matrix
					   G4ThreeVector(0,-floorBoxDisplacement,0)); // translation

      // need to create a container for the tunnel + floor that only just contains it
      // need to do the same trick again - prepare small floor container segment and union
      // it with tunnel container. Container has to be a wee bit bigger so can't use the same
      // solids sadly.

      // floor container cylinder has to definitely overlaps with outside cylinder -> tunnel1*1.1
      G4VSolid* floorContainerCylinder = new G4Tubs(name + "_floor_cont_cyl_solid", // name
						    0,                              // inner radius
						    tunnel1*1.1,                    // outer radius
						    length*0.5,                     // z half length
						    0,                              // start angle
						    CLHEP::twopi);                  // sweep angle

      G4VSolid* floorContainerBox = new G4Box(name + "_floor_cont_box_solid",  // name
					      floorBoxRadius,                  // x half width
					      floorBoxRadius,                  // y half width
					      length);                         // z half length
      // z long for unambiguous intersection

      // calculate box container offset - should be just above floor by lengthsafety (floor actually lowered
      // by length safety a la rest of geometry to fit within its dimensions)
      G4double floorBoxContDisp = floorBoxDisplacement - lengthSafety;
      G4VSolid* floorContainerSolid = new G4IntersectionSolid(name + "_floor_cont_solid",           // name
							      floorContainerCylinder,               // this
							      floorContainerBox,                    // minus this,
							      0,                                    // rotation matrix
							      G4ThreeVector(0,-floorBoxContDisp,0));// translation

      G4VSolid* tunnelContainerSolid = new G4Tubs(name + "_tunnel_solid",    // name
						  tunnel1,                   // inner radius
						  containerRadius,           // outer radius
						  length*0.5,                // z half length
						  0,                         // start angle
						  CLHEP::twopi);             // sweep angle

      containerSolid = new G4UnionSolid(name + "_container_solid", // name
					tunnelContainerSolid,      // this
					floorContainerSolid);      // plus this
    }
  else
    {
      containerSolid = new G4Tubs(name + "_tunnel_container_solid", // name
				  tunnel1,                          // inner radius
				  containerRadius,                  // outer radius,
				  length,                           // z half length
				  0,                                // start angle
				  CLHEP::twopi);                    // sweep angle
    } 

  CommonFinalConstruction(name, length, tunnelMaterial, tunnelSoilMaterial, containerRadius);
  
  return tunnelSection; // member variable geometry component that's assembled in base class
}


BDSGeometryComponent* BDSTunnelFactoryCircular::CreateTunnelSectionAngledInOut(G4String      name,
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
									       G4double      /*tunnel2*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // tidy up things from previous usage if any - base class method
  TidyUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);

  std::pair<G4ThreeVector,G4ThreeVector> faces = CalculateFaces(angleIn, angleOut);
  G4ThreeVector inputface  = faces.first;
  G4ThreeVector outputface = faces.second;
  
  // build the solids
  tunnelSolid = new G4CutTubs(name + "_tunnel_solid",    // name
			      tunnel1,                   // inner radius
			      tunnel1 + tunnelThickness, // outer radius
			      length*0.5 - lengthSafety, // z half length
			      0,                         // start angle
			      CLHEP::twopi,              // sweep angle
			      inputface,                 // input face normal vector
			      outputface);               // output face normal vector

  G4double soilInnerR = tunnel1 + tunnelThickness + lengthSafety;
  G4double soilOuterR = soilInnerR + tunnelSoilThickness;
  soilSolid   = new G4CutTubs(name + "_soil_solid",      // name
			      soilInnerR,                // inner radius
			      soilOuterR,                // outer radius
			      length*0.5 - lengthSafety, // z half length
			      0,                         // start angle
			      CLHEP::twopi,              // sweep angle 
			      inputface,                 // input face normal vector
			      outputface);               // output face normal vector

  G4double containerRadius = soilOuterR + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      // these three lines are a repeat of the same part in the first function (~L211)
      G4double floorBoxRadius = 1.5*tunnel1; // will definitely encompass the tunnel
      G4double floorCylinderRadius = tunnel1 - lengthSafety; // so it fits within the tunnel
      G4double floorBoxDisplacement = tunnelFloorOffset + floorBoxRadius + lengthSafety;
      
      G4VSolid* floorCylinder = new G4CutTubs(name + "_floor_cylinder_solid", // name
					      0,                              // inner radius
					      floorCylinderRadius,            // outer radius
					      length*0.5 - lengthSafety,      // z half length
					      0,                              // start angle
					      CLHEP::twopi,                   // sweep angle
					      inputface,                      // input face normal vector
					      outputface);                    // output face normal vector

      G4VSolid* floorBox      = new G4Box(name + "_floor_box_solid",  // name
					  1.5*tunnel1,                // x half width
					  1.5*tunnel1,                // y half width
					  length);                    // z half length
      // z long for unambiguous intersection
      
      floorSolid = new G4IntersectionSolid(name + "_floor_solid",                      // name
					   floorCylinder,                              // this
					   floorBox,                                   // minus this,
					   0,                                          // rotation matrix
					   G4ThreeVector(0,-floorBoxDisplacement,0));  // translation
      
      // floor container cylinder has to definitely overlaps with outside cylinder -> tunnel1*1.1
      G4VSolid* floorContainerCylinder = new G4CutTubs(name + "_floor_cont_cyl_solid", // name
						       0,                              // inner radius
						       tunnel1*1.1,                    // outer radius
						       length*0.5,                     // z half length
						       0,                              // start angle
						       CLHEP::twopi,                   // sweep angle
						       inputface,                      // input face normal
						       outputface);                    // output face normal

      G4VSolid* floorContainerBox = new G4Box(name + "_floor_cont_box_solid",  // name
					      1.5*tunnel1,                     // x half width
					      1.5*tunnel1,                     // y half width
					      length);                         // z half length
      // floor container box z long for unambiguous intersection

      // calculate box container offset - should be just above floor by lengthsafety (floor actually lowered
      // by length safety a la rest of geometry to fit within its dimensions)
      G4double floorBoxContDisp = floorBoxDisplacement - lengthSafety;
      G4VSolid* floorContainerSolid = new G4IntersectionSolid(name + "_floor_cont_solid",           // name
							      floorContainerCylinder,               // this
							      floorContainerBox,                    // minus this,
							      0,                                    // rotation matrix
							      G4ThreeVector(0,-floorBoxContDisp,0));// translation

      G4VSolid* tunnelContainerSolid = new G4CutTubs(name + "_tunnel_solid",    // name
						     tunnel1 - lengthSafety,    // inner radius
						     containerRadius,           // outer radius
						     length*0.5,                // z half length
						     0,                         // start angle
						     CLHEP::twopi,              // sweep angle
						     inputface,                 // input face normal vector
						     outputface);               // output face normal vector

      containerSolid = new G4UnionSolid(name + "_container_solid", // name
					tunnelContainerSolid,      // this
					floorContainerSolid);      // plus this
    }
  else
    {
      containerSolid = new G4CutTubs(name + "_tunnel_container_solid", // name
				     tunnel1,                          // inner radius
				     containerRadius,                  // outer radius,
				     length,                           // z half length
				     0,                                // start angle
				     CLHEP::twopi,                     // sweep angle
				     inputface,                        // input face normal vector
				     outputface);                      // output face normal vector
    } 

  CommonFinalConstruction(name, length, tunnelMaterial, tunnelSoilMaterial, containerRadius);

  return tunnelSection;
}

/// functions below here are private to this particular factory
void BDSTunnelFactoryCircular::TestInputParameters(G4double&    length,
						   G4double&    tunnelThickness,
						   G4double&    tunnelSoilThickness,
						   G4Material*& tunnelMaterial,
						   G4Material*& tunnelSoilMaterial,
						   G4double&    tunnelFloorOffset,
						   G4double&    tunnel1)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);
  
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel->floorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel->aper1;}
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSGeometryComponent* BDSTunnelFactoryCircular::CommonFinalConstruction(G4String    name,
									G4double    length,
									G4Material* tunnelMaterial,
									G4Material* tunnelSoilMaterial,
									G4double    containerRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSTunnelFactoryBase::CommonConstruction(name,
					   tunnelMaterial,
					   tunnelSoilMaterial,
					   length);

  // record extents
  std::pair<double,double> extX = std::make_pair(-containerRadius, containerRadius);
  std::pair<double,double> extY = std::make_pair(-containerRadius, containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  BDSGeometryComponent* aTunnelSegment = new BDSGeometryComponent(containerSolid,
								  containerLV,
								  extX,
								  extY,
								  extZ);

  return aTunnelSegment;
}
