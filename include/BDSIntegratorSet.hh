#ifndef BDSINTEGRATORSET_H
#define BDSINTEGRATORSET_H

#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals

struct BDSIntegratorSet;

namespace BDS
{
  /// @{ Return the appropraiate set of integrators to use for each magnet type.
  BDSIntegratorSet GetIntegratorSet(G4String             set);
  BDSIntegratorSet GetIntegratorSet(BDSIntegratorSetType set);
  /// @}
}

/**
 * @brief Which integrator to use for each type of magnet / field object
 * 
 * @author Laurie Nevay
 */

struct BDSIntegratorSet
{
  BDSIntegratorSet(BDSIntegratorType solenoidIn,
		   BDSIntegratorType dipoleIn,
		   BDSIntegratorType quadrupoleIn,
		   BDSIntegratorType sextupoleIn,
		   BDSIntegratorType octupoleIn,
		   BDSIntegratorType decapoleIn,
		   BDSIntegratorType multipoleIn,
		   BDSIntegratorType muonspoilerIn,
		   BDSIntegratorType rfcavityIn,
		   BDSIntegratorType rfIn,
		   BDSIntegratorType generalIn):
    solenoid(solenoidIn),
    dipole(dipoleIn),
    quadrupole(quadrupoleIn),
    sextupole(sextupoleIn),
    octupole(octupoleIn),
    decapole(decapoleIn),
    multipole(multipoleIn),
    muonspoiler(muonspoilerIn),
    rfcavity(rfcavityIn),
    rf(rfIn),
    general(generalIn)
  {;}

  BDSIntegratorType solenoid;
  BDSIntegratorType dipole;
  BDSIntegratorType quadrupole;
  BDSIntegratorType sextupole;
  BDSIntegratorType octupole;
  BDSIntegratorType decapole;
  BDSIntegratorType multipole;
  BDSIntegratorType muonspoiler;
  BDSIntegratorType rfcavity;
  BDSIntegratorType rf;
  BDSIntegratorType general;
};


#endif
