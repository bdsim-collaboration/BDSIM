#include "BDSInterpolatorType.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals

#include <map>
#include <string>

// dictionary for BDSInterpolatorType for reflexivity
template<>
std::map<BDSInterpolatorType, std::string>* BDSInterpolatorType::dictionary =
  new std::map<BDSInterpolatorType, std::string> ({
      {BDSInterpolatorType::nearest2D,  "nearest2D"},
      {BDSInterpolatorType::linear2D,   "linear2D"},
      {BDSInterpolatorType::nearest3D,  "nearest3D"},
      {BDSInterpolatorType::linear3D,   "linear3D"}
    });

BDSInterpolatorType BDS::DetermineInterpolatorType(G4String interpolatorType)
{
  std::map<G4String, BDSInterpolatorType> types;
  types["nearest2D"] = BDSInterpolatorType::nearest2D;
  types["linear2D"]  = BDSInterpolatorType::linear2D;
  types["nearest3D"] = BDSInterpolatorType::nearest3D;
  types["linear3D"]  = BDSInterpolatorType::linear3D;

  interpolatorType.toLower();

  auto result = types.find(interpolatorType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << interpolatorType << " is not a valid field type" << G4endl;

      G4cout << "Available interpolator types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined interpolator type to be " << result->second << G4endl;
#endif
  return result->second;
}
