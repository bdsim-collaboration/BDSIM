#ifndef BDSMAGNETOUTERFACTORYPOLESCIRCULAR_H
#define BDSMAGNETOUTERFACTORYPOLESCIRCULAR_H

#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuterFactoryPolesBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
//#include "G4Colour.hh"
//#include "G4Material.hh"
#include "G4VSolid.hh"

/**
 * @brief Factory class for outer volume of magnets. Produces magnets
 * with 2N-poles around the beampipe with a circular yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */


class BDSMagnetOuterFactoryPolesCircular: public BDSMagnetOuterFactoryPolesBase
{
public:
  static BDSMagnetOuterFactoryPolesCircular* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactoryPolesCircular();

private:
  /// Private constructor as singleton - nothing special here - all in parent class
  BDSMagnetOuterFactoryPolesCircular(){;}; 
  static BDSMagnetOuterFactoryPolesCircular* _instance;
};

#endif
