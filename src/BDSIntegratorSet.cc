#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals

BDSIntegratorSet BDS::GetIntegratorSet(G4String sets)
{
  return BDS::GetIntegratorSet(BDS::DetermineIntegratorSetType(sets));
}

BDSIntegratorSet BDS::GetIntegratorSet(BDSIntegratorSetType set)
{
  BDSIntegratorSet geant4 = BDSIntegratorSet(BDSIntegratorType::g4classicalrk4, // solenoid
					     BDSIntegratorType::g4classicalrk4, // dipole
					     BDSIntegratorType::g4classicalrk4, // quadrupole
					     BDSIntegratorType::g4classicalrk4, // sextupole
					     BDSIntegratorType::g4classicalrk4, // octupole
					     BDSIntegratorType::g4classicalrk4, // decapole
					     BDSIntegratorType::g4classicalrk4, // multipole
					     BDSIntegratorType::g4classicalrk4, // muonspoiler
					     BDSIntegratorType::g4classicalrk4, // rfcavity
					     BDSIntegratorType::g4classicalrk4, // rf
					     BDSIntegratorType::g4classicalrk4);// general
  
  BDSIntegratorSet bdsim  = BDSIntegratorSet(BDSIntegratorType::solenoid,       // solenoid
					     BDSIntegratorType::dipole,         // dipole
					     BDSIntegratorType::quadrupole,     // quadrupole
					     BDSIntegratorType::sextupole,      // sextupole
					     BDSIntegratorType::octupole,       // octupole
					     BDSIntegratorType::decapole,       // decapole
					     BDSIntegratorType::multipole,      // multipole
					     BDSIntegratorType::g4classicalrk4, // muonspoiler
					     BDSIntegratorType::g4classicalrk4, // rfcavity
					     BDSIntegratorType::g4classicalrk4, // rf
					     BDSIntegratorType::g4classicalrk4);// general

  switch (set.underlying())
    {
    case BDSIntegratorSetType::geant4:
      {return geant4; break;}
    case BDSIntegratorSetType::bdsim:
      {return bdsim;  break;}
    default:
      {return bdsim;  break;}
    }
}
  
