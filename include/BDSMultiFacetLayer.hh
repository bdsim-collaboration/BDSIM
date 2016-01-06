#ifndef BDSMULTIFACETLAYER_H
#define BDSMULTIFACETLAYER_H 

#include "globals.hh"

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
  G4double _gapWidth;
  G4double _nFacets;
};

#endif
