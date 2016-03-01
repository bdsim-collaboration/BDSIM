#include "BDSSamplerType.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals

#include <map>
#include <string>

// dictionary for BDSSamplerType
template<>
std::map<BDSSamplerType, std::string>* BDSSamplerType::dictionary =
  new std::map<BDSSamplerType, std::string> ({
  {BDSSamplerType::none,     "none"},
  {BDSSamplerType::plane,    "plane"},
  {BDSSamplerType::cylinder, "cylinder"}
});

BDSSamplerType BDS::DetermineSamplerType(G4String samplerType)
{
  std::map<G4String, BDSSamplerType> types;
  types["none"]     = BDSSamplerType::none;
  types["plane"]    = BDSSamplerType::plane;
  types["cylinder"] = BDSSamplerType::cylinder;

  samplerType.toLower();

  auto result = types.find(samplerType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << samplerType << " is not a valid sampler type" << G4endl;
      // don't need to output types here as only for developer
      exit(1);
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined sampler type to be " << result->second << G4endl;
#endif
  return result->second;
}
