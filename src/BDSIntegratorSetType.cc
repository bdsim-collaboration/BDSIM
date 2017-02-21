#include "BDSDebug.hh"
#include "BDSIntegratorSetType.hh"

#include <map>
#include <string>

template<>
std::map<BDSIntegratorSetType, std::string>* BDSIntegratorSetType::dictionary =
  new std::map<BDSIntegratorSetType, std::string> ({
      {BDSIntegratorSetType::bdsim,       "bdsim"},
      {BDSIntegratorSetType::bdsimnew,    "bdsimnew"},
      {BDSIntegratorSetType::geant4exact, "geant4exact"},
      {BDSIntegratorSetType::geant4rk4,   "geant4rk4"}
    });

BDSIntegratorSetType BDS::DetermineIntegratorSetType(G4String integratorSet)
{
  std::map<G4String, BDSIntegratorSetType> types;
  types["bdsim"]       = BDSIntegratorSetType::bdsim;
  types["bdsimnew"] = BDSIntegratorSetType::bdsimnew;
  types["geant4exact"] = BDSIntegratorSetType::geant4exact;
  types["geant4rk4"]   = BDSIntegratorSetType::geant4rk4;

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
