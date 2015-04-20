#ifndef BDSMAGNETOUTERFACTORYPOLESFACET_H
#define BDSMAGNETOUTERFACTORYPOLESFACET_H

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
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */


class BDSMagnetOuterFactoryPolesFacet: public BDSMagnetOuterFactoryPolesBase
{
public:
  static BDSMagnetOuterFactoryPolesFacet* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactoryPolesFacet();

private:
  /// Private constructor as singleton - nothing special here - all in parent class
  BDSMagnetOuterFactoryPolesFacet(){;}; 
  static BDSMagnetOuterFactoryPolesFacet* _instance;

  /// Create pole for magnet of order N where npoles = Nx2
  virtual void CreatePoleSolid(G4String      name,                 // name
			       G4double      length,               // length [mm]
			       G4int         order);               // Nx2 poles
  
  /// Create yoke that connects poles and container to put them in
  virtual void CreateYokeAndContainerSolid(G4String      name,
					   G4double      length,
					   G4int         order);
  
};

#endif
