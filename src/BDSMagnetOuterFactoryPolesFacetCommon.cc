#include "BDSDebug.hh"
#include "BDSMagnetOuterFactoryPolesFacetCommon.hh"

#include "globals.hh"  // geant4 globals / types
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"


BDSMagnetOuterFactoryPolesFacetCommon::BDSMagnetOuterFactoryPolesFacetCommon(G4double factorIn):
  BDSMagnetOuterFactoryPolesBase(/*poleStopFactor=*/2), factor(factorIn)
{;}

BDSMagnetOuterFactoryPolesFacetCommon::~BDSMagnetOuterFactoryPolesFacetCommon()
{;}

void BDSMagnetOuterFactoryPolesFacetCommon::CreateYokeAndContainerSolid(G4String name,
									G4double length,
									G4int    order,
									G4double magnetContainerLength)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4double zPlanes[2]      = {-length*0.5,      length*0.5};
  G4double zPlanesLong[2]  = {-length,          length}; // for intersections
  G4double innerRadii[2]   = {yokeStartRadius,  yokeStartRadius};
  G4double outerRadii[2]   = {yokeFinishRadius, yokeFinishRadius};
  G4double zeroRadii[2]    = {0,0};
  G4double poleEnd         = yokeStartRadius - lengthSafetyLarge;
  G4double poleEndRadii[2] = {poleEnd,          poleEnd};

  const G4double polyStartAngle = CLHEP::halfpi;
  
  // The start angle for G4Polyhedra lies on a vertex / point and is at the top
  // in the positive y direction. 
  yokeSolid = new G4Polyhedra(name + "_yoke_solid",    // name
			      polyStartAngle,          // start angle
			      CLHEP::twopi,            // sweep angle
			      factor*2*order,          // number of sides
			      2,                       // number of z planes
			      zPlanes,                 // z plane z coordinates
			      innerRadii,
			      outerRadii);

  // pole intersection solid
  // note the start angle is 0 here instead of polyStartAngle as a pole is always
  // built upright and then rotated to the correct position.
  poleIntersectionSolid = new G4Polyhedra(name + "_yoke_intersection_solid", // name
					  segmentAngle*0.5,                  // start angle
					  CLHEP::twopi,                      // sweep angle
					  factor*2*order,                    // number of sides
					  2,                                 // number of z planes
					  zPlanesLong,                       // z plane z coordinates
					  zeroRadii,
					  poleEndRadii);
  allSolids.push_back(poleIntersectionSolid);
					  
  
  G4double contInnerRadii[2] = {0, 0}; // solid polyhedra
  G4double contOuterRadii[2] = {yokeFinishRadius + lengthSafety, yokeFinishRadius + lengthSafety};
  G4VSolid* containerOuterSolid = new G4Polyhedra(name + "_container_outer_solid", // name
						  polyStartAngle,                  // start angle
						  CLHEP::twopi,                    // sweep angle
						  factor*2*order,                  // number of sides
						  2,                               // number of z planes
						  zPlanes,                         // z plane z coordinates
						  contInnerRadii,
						  contOuterRadii);

  G4VSolid* containerInnerSolid = new G4Tubs(name + "_container_solid",    // name
					     0,                            // start radius
					     poleStartRadius,              // finish radius
					     length,                       // z half length
					     0,                            // start angle
					     CLHEP::twopi);                // sweep angle
  // z long for unambiguous subtraction
  allSolids.push_back(containerOuterSolid);
  allSolids.push_back(containerInnerSolid);
  
  containerSolid = new G4SubtractionSolid(name + "_container_solid", // name
					  containerOuterSolid,       // this
					  containerInnerSolid);      // minus this with no translation or rotation
  
  G4double magContOuterRadii[2] = {magnetContainerRadius, magnetContainerRadius};
  magnetContainerSolid = new G4Polyhedra(name + "_container_solid", // name
					 polyStartAngle,            // start angle
					 CLHEP::twopi,              // sweep angle
					 factor*2*order,            // number of sides
					 2,                         // number of z planes
					 zPlanes,                   // z plane z coordinates
					 contInnerRadii,
					 magContOuterRadii);
  
  magContExtentX = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentY = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentX = std::make_pair(-magnetContainerLength*0.5, magnetContainerLength*0.5); 
}
