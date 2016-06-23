#include "BDSDebug.hh"
#include "BDSMagnetOuterFactoryPolesFacet.hh"

#include "globals.hh"  // geant4 globals / types

#include "G4Box.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

BDSMagnetOuterFactoryPolesFacet* BDSMagnetOuterFactoryPolesFacet::_instance = nullptr;

BDSMagnetOuterFactoryPolesFacet* BDSMagnetOuterFactoryPolesFacet::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesFacet();}
  return _instance;
}

BDSMagnetOuterFactoryPolesFacet::BDSMagnetOuterFactoryPolesFacet():
  BDSMagnetOuterFactoryPolesBase(/*poleStopFactor=*/2)
{;}

BDSMagnetOuterFactoryPolesFacet::~BDSMagnetOuterFactoryPolesFacet()
{
  _instance = nullptr;
}

void BDSMagnetOuterFactoryPolesFacet::CreateYokeAndContainerSolid(G4String name,
								  G4double length,
								  G4int    order,
								  G4double magnetContainerLength)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4double zPlanes[2] = {-length*0.5, length*0.5};
  G4double innerRadii[2] = {yokeStartRadius, yokeStartRadius};
  G4double outerRadii[2] = {yokeFinishRadius, yokeFinishRadius};
  
  yokeSolid = new G4Polyhedra(name + "_yoke_solid",    // name
			      CLHEP::pi*0.5,    // start angle
			      CLHEP::twopi,            // sweep angle
			      2*order,                 // number of sides
			      2,                       // number of z planes
			      zPlanes,                 // z plane z coordinates
			      innerRadii,
			      outerRadii);
  
  G4double contInnerRadii[2] = {0, 0}; // solid polyhedra
  G4double contOuterRadii[2] = {yokeFinishRadius + lengthSafety, yokeFinishRadius + lengthSafety};
  G4VSolid* containerOuterSolid = new G4Polyhedra(name + "_container_outer_solid", // name
						  CLHEP::pi*0.5,                   // start angle
						  CLHEP::twopi,                    // sweep angle
						  2*order,                         // number of sides
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
					 CLHEP::pi*0.5,                   // start angle
					 CLHEP::twopi,                    // sweep angle
					 2*order,                         // number of sides
					 2,                               // number of z planes
					 zPlanes,                         // z plane z coordinates
					 contInnerRadii,
					 magContOuterRadii);
  
  magContExtentX = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentY = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentX = std::make_pair(-magnetContainerLength*0.5, magnetContainerLength*0.5); 
}
