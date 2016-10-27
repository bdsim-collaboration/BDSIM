#ifndef BDSINTEGRATORTYPE_H
#define BDSINTEGRATORTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for integrators.
 * 
 * @author Laurie Nevay
 */

struct integratortypes_def
{

  enum type {none, solenoid, dipole, quadrupole, sextupole,
	     multipole, octupole, decapole, dipolefringe,
	     g4cashkarprkf45, g4classicalrk4, g4constrk4, g4expliciteuler,
	     g4impliciteuler, g4simpleheum, g4simplerunge,
	     g4exacthelixstepper, g4helixexpliciteuler, g4helixheum,
	     g4heliximpliciteuler, g4helixmixedstepper, g4helixsimplerunge,
	     g4nystromrk4, g4rkg3stepper};
};

typedef BDSTypeSafeEnum<integratortypes_def,int> BDSIntegratorType;

namespace BDS
{
  /// Function that determines enum from string (case-insensitive).
  BDSIntegratorType DetermineIntegratorType(G4String integratorType);
}

#endif
