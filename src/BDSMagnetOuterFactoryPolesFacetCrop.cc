#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"

#include "globals.hh"  // geant4 globals / types

#include "G4Box.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>

BDSMagnetOuterFactoryPolesFacetCrop* BDSMagnetOuterFactoryPolesFacetCrop::_instance = 0;

BDSMagnetOuterFactoryPolesFacetCrop* BDSMagnetOuterFactoryPolesFacetCrop::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactoryPolesFacetCrop();}
  return _instance;
}

BDSMagnetOuterFactoryPolesFacetCrop::~BDSMagnetOuterFactoryPolesFacetCrop()
{
  _instance = 0;
}

// NOTE unforunately can't get this through inheritance as BDSMagnetOuterFactoryPolesFacet
// is a singleton with private constructor - so this method is duplicated
void BDSMagnetOuterFactoryPolesFacetCrop::CreatePoleSolid(G4String     name,
							  G4double     length,
							  G4int        order)
{
  // use base class to do all the work, then modify the pole by cropping
  // it with a box to get the right shape
  
  // set pole length to be longer so we have unambiguous subtraction
  G4double tempPoleFinishRadius = poleFinishRadius; // copy to set back later
  poleFinishRadius = 2*poleFinishRadius;
  
  // base class method will use poleFinishRadius
  BDSMagnetOuterFactoryPolesBase::CreatePoleSolid(name,length,order);
  
  poleFinishRadius = tempPoleFinishRadius; // set it back to what it was

  G4VSolid* baseClassPoleSolid = poleSolid;

  // for box sizes we need something roughly adaptive to the component size
  // but just needs to be big - don't want to hard code anything so use
  // poleFinishRadius - a bigger component will entail bigger poleFinishRadius
  G4VSolid* subtractionBox = new G4Box(name + "_subtraction_box",  // name
				       poleFinishRadius,           // x half width
				       poleFinishRadius,           // y half width
				       length);                    // z half width
  // z half width is full length for unambiguous subtraction

  // note translation is to centre of box which is nominal poleFinishRadius + half width
  // of box which is poleFinishRadius also - so tranlation is 2*poleFinishRadius
  G4ThreeVector boxTranslation(2*poleFinishRadius,0,0);
  poleSolid = new G4SubtractionSolid(name + "_pole_solid",        // name
				     baseClassPoleSolid,          // solid 1
				     subtractionBox,              // solid 2 - subtract this one
				     0,                           // rotation
				     boxTranslation);
				     
}

void BDSMagnetOuterFactoryPolesFacetCrop::CreateYokeAndContainerSolid(G4String      name,
								      G4double      length,
								      G4int         order)
{
  G4double segmentAngle = CLHEP::twopi / (2*order);
  
  G4double zPlanes[2] = {-length*0.5, length*0.5};
  std::vector<G4double> innerRadii;
  std::vector<G4double> outerRadii;
  for (G4int i = 0; i < order*4; ++i)
    {
      innerRadii.push_back(yokeStartRadius);
      outerRadii.push_back(yokeFinishRadius);
    }
  
  yokeSolid = new G4Polyhedra(name + "_yoke_solid",    // name
			      CLHEP::pi*0.5 + segmentAngle*0.25,    // start angle
			      CLHEP::twopi,            // sweep angle
			      4*order,                 // number of sides
			      2,                       // number of z planes
			      zPlanes,                 // z plane z coordinates
			      innerRadii.data(),
			      outerRadii.data());

  // note container must have hole in it for the beampipe to fit in!
  // poled geometry doesn't fit tightly to beampipe so can alays use a circular aperture
  G4double yokeExtent   = yokeFinishRadius / cos(segmentAngle*0.25);
  
  containerSolid = new G4Tubs(name + "_container_solid",       // name
			      poleStartRadius,                 // start radius
			      yokeExtent + lengthSafety,       // finish radius
			      length*0.5,                      // z half length
			      0,                               // start angle
			      CLHEP::twopi);                   // sweep angle


}
