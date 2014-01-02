#ifndef BDSMultiFacetLayer_h
#define BDSMultiFacetLayer_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerSD.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4TwoVector.hh"
#include "BDSScreenLayer.hh"
#include "BDSLensFacet.hh"
#include <vector>

class BDSMultiFacetLayer : public BDSScreenLayer 
{
public:
  BDSMultiFacetLayer(G4String name, BDSLensFacet* facet, G4double gapWidth, G4double nFacets);
  ~BDSMultiFacetLayer();

private:
  void computeDimensions();
  void build();
  void buildScreen();
  void placeFacet(G4double xPos);
  void placeFacets();
  BDSLensFacet* _facet;
  std::vector<G4ThreeVector> _facetPos;
  G4double _nFacets;
  G4double _gapWidth;
};

#endif
