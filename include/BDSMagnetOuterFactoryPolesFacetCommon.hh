#ifndef BDSMAGNETOUTERFACTORYPOLESFACETCOMMON_H
#define BDSMAGNETOUTERFACTORYPOLESFACETCOMMON_H

#include "BDSMagnetOuterFactoryPolesBase.hh"

#include "globals.hh"           // geant4 globals / types

/**
 * @brief Factory class for outer volume of magnets. Produces magnets
 * with 2N-poles around the beampipe with a faceted yoke - the yoke 
 * is perpendiular to the pole direction where the pole joins the yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * This class provides the common code for 'facet' and 'facetcrop'
 * styles as there's only one number different in the yoke code.
 *
 * @author Laurie Nevay
 */


class BDSMagnetOuterFactoryPolesFacetCommon: public BDSMagnetOuterFactoryPolesBase
{
public:
  BDSMagnetOuterFactoryPolesFacetCommon(G4double factor);
  
  virtual ~BDSMagnetOuterFactoryPolesFacetCommon();

private:
  /// Private constructor to force use of supplied one.
  BDSMagnetOuterFactoryPolesFacetCommon();
  
  /// Create yoke that connects poles and container to put them in
  virtual void CreateYokeAndContainerSolid(G4String name,
					   G4double length,
					   G4int    order,
					   G4double magnetContainerRadius);

  /// Factor by which number of polyhedra vertices is multiplied by.
  G4double factor;
};

#endif
