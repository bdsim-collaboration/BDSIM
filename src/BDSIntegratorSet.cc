#include "BDSDebug.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals

#include <map>
#include <string>

template<>
std::map<BDSIntegratorSetType, std::string>* BDSIntegratorSetType::dictionary =
  new std::map<BDSIntegratorSetType, std::string> ({
       {BDSIntegratorSetType::geant4, "geant4"}
      ,{BDSIntegratorSetType::bdsim,  "bdsim"}
    });

BDSIntegratorSetType BDS::DetermineIntegratorSetType(G4String integratorSet)
{
  std::map<G4String, BDSIntegratorSetType> types;
  types["geant4"] = BDSIntegratorSetType::geant4;
  types["bdsim"]  = BDSIntegratorSetType::bdsim;

  integratorSet.toLower();

  auto result = types.find(integratorSet);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << integratorSet << " is not a valid integrator set" << G4endl;

      G4cout << "Available sets are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined integrator set to be " << result->second << G4endl;
#endif
  return result->second;
}

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
					     BDSIntegratorType::g4classicalrk4, // multipole
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
  
