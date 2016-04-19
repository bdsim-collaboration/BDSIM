#ifndef BDSINTEGRATORSET_H
#define BDSINTEGRATORSET_H

#include "BDSIntegratorType.hh"

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
