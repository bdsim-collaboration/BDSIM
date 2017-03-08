#include "BDSDebug.hh"
#include "BDSIntegratorSetType.hh"

#include <map>
#include <string>

template<>
std::map<BDSIntegratorSetType, std::string>* BDSIntegratorSetType::dictionary =
  new std::map<BDSIntegratorSetType, std::string> ({
      {BDSIntegratorSetType::geant4,   "geant4"},
      {BDSIntegratorSetType::bdsimone, "bdsimone"},
      {BDSIntegratorSetType::bdsimtwo, "bdsimtwo"}
    });

BDSIntegratorSetType BDS::DetermineIntegratorSetType(G4String integratorSet)
{
  std::map<G4String, BDSIntegratorSetType> types;
  types["geant4"]   = BDSIntegratorSetType::geant4;
  types["bdsim"]    = BDSIntegratorSetType::bdsimone; // alias for bdsim one
  types["bdsimone"] = BDSIntegratorSetType::bdsimone;
  types["bdsimtwo"] = BDSIntegratorSetType::bdsimtwo;

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
