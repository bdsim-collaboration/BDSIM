#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryCircular.hh"

#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSTunnelInfo.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
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
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);

  // build the solids
  tunnelSolid = new G4Tubs(name + "_tunnel_solid",    // name
			   tunnel1,                   // inner radius
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
      G4VSolid* floorCylinder = new G4Tubs(name + "_floor_cylinder_solid", // name
					   0,                              // inner radius
					   tunnel1,                        // outer radius
					   length*0.5 - lengthSafety,      // z half length
					   0,                              // start angle
					   CLHEP::twopi);                  // sweep angle

      G4VSolid* floorBox      = new G4Box(name + "_floor_box_solid",  // name
					  1.5*tunnel1,                // x half width
					  1.5*tunnel1,                // y half width
					  length);                    // z half length
      // z long for unambiguous subtraction
      
      floorSolid = new G4SubtractionSolid(name + "_floor_solid",  // name
					  floorBox,          // this
					  floorCylinder,               // minus this,
					  0,                      // rotation matrix
					  floorOffset);           // translation
      
      // container should contain floor as well
      // build container while we have necessary subtraction solids
      G4VSolid* floorContainerCylinder = new G4Tubs(name + "_floor_cont_cyl_solid", // name
						    0,                              // inner radius
						    tunnel1*1.1,                    // outer radius
						    length*0.5,                     // z half length
						    0,                              // start angle
						    CLHEP::twopi);                  // sweep angle

      G4VSolid* floorContainerBox = new G4Box(name + "_floor_cont_box_solid",  // name
					      1.5*tunnel1,                     // x half width
					      1.5*tunnel1,                     // y half width
					      length);                         // z half length
      // z long for unambiguous subtraction
      G4ThreeVector* offset = new G4ThreeVector(0,tunnelFloorOffset-lengthSafety,0);
      G4VSolid* floorContainerSolid = new G4SubtractionSolid(name + "_floor_cont_solid",  // name
							     floorContainerCylinder,      // this
							     floorContainerBox,           // minus this,
							     0,                           // rotation matrix
							     *offset);                    // translation

      G4VSolid* tunnelContainerSolid = new G4Tubs(name + "_tunnel_solid",    // name
						  tunnel1 - lengthSafety,    // inner radius
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
				  tunnel1 - lengthSafety,           // inner radius
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
      G4VSolid* floorCylinder = new G4CutTubs(name + "_floor_cylinder_solid", // name
					      0,                              // inner radius
					      tunnel1,                        // outer radius
					      length*0.5 - lengthSafety,      // z half length
					      0,                              // start angle
					      CLHEP::twopi,                   // sweep angle
					      inputface,                      // input face normal vector
					      outputface);                    // output face normal vector

      G4VSolid* floorBox      = new G4Box(name + "_floor_box_solid",  // name
					  1.5*tunnel1,                // x half width
					  1.5*tunnel1,                // y half width
					  length);                    // z half length
      // z long for unambiguous subtraction
      
      floorSolid = new G4SubtractionSolid(name + "_floor_solid",    // name
					  floorCylinder,            // this
					  floorBox,                 // minus this,
					  0,                        // rotation matrix
					  floorOffset);             // translation
      
      // container should contain floor as well
      // build container while we have necessary subtraction solids
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
      // z long for unambiguous subtraction
      G4ThreeVector* offset = new G4ThreeVector(0,tunnelFloorOffset-lengthSafety,0);
      G4VSolid* floorContainerSolid = new G4SubtractionSolid(name + "_floor_cont_solid",  // name
							     floorContainerCylinder,      // this
							     floorContainerBox,           // minus this,
							     0,                           // rotation matrix
							     *offset);                    // translation

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
				     tunnel1 - lengthSafety,           // inner radius
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

  BDSTunnelInfo defaultModel = BDSGlobalConstants::Instance()->GetTunnelInfo();
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel.tunnelFloorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel.tunnel1;}
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
