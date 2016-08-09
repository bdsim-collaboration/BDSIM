#ifndef BDSMAGNETOUTERFACTORYPOLESFACET_H
#define BDSMAGNETOUTERFACTORYPOLESFACET_H

#include "BDSMagnetOuterFactoryPolesFacetCommon.hh"

#include "globals.hh" // geant4 globals / types

/**
 * @brief Factory class for outer volume of magnets. 
 *
 * Produces magnets
 * with 2N-poles around the beampipe with a faceted yoke - the yoke 
 * is perpendiular to the pole direction where the pole joins the yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */


class BDSMagnetOuterFactoryPolesFacet: public BDSMagnetOuterFactoryPolesFacetCommon
{
public:
  /// Singleton accessor
  static BDSMagnetOuterFactoryPolesFacet* Instance();
  
  virtual ~BDSMagnetOuterFactoryPolesFacet();

private:
  /// Private constructor as singleton
  BDSMagnetOuterFactoryPolesFacet();

  /// Singleton instance.
  static BDSMagnetOuterFactoryPolesFacet* _instance;
};

#endif
