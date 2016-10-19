#include "BDSInterpolatorType.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals

#include <map>
#include <string>

// dictionary for BDSInterpolatorType for reflexivity
template<>
std::map<BDSInterpolatorType, std::string>* BDSInterpolatorType::dictionary =
  new std::map<BDSInterpolatorType, std::string> ({
      {BDSInterpolatorType::nearest2d,  "nearest1d"},
      {BDSInterpolatorType::nearest2d,  "nearest2d"},
      {BDSInterpolatorType::nearest3d,  "nearest3d"},
      {BDSInterpolatorType::nearest4d,  "nearest4d"},
      {BDSInterpolatorType::linear2d,   "linear1d"},
      {BDSInterpolatorType::linear2d,   "linear2d"},
      {BDSInterpolatorType::linear3d,   "linear3d"},
      {BDSInterpolatorType::linear4d,   "linear4d"},
      {BDSInterpolatorType::cubic1d,    "cubic1d"},
      {BDSInterpolatorType::cubic2d,    "cubic2d"},
      {BDSInterpolatorType::cubic3d,    "cubic3d"},
      {BDSInterpolatorType::cubic4d,    "cubic4d"}
    });

BDSInterpolatorType BDS::DetermineInterpolatorType(G4String interpolatorType)
{
  std::map<G4String, BDSInterpolatorType> types;
  types["nearest1d"] = BDSInterpolatorType::nearest1d;
  types["nearest2d"] = BDSInterpolatorType::nearest2d;
  types["nearest3d"] = BDSInterpolatorType::nearest3d;
  types["nearest4d"] = BDSInterpolatorType::nearest4d;
  types["linear1d"]  = BDSInterpolatorType::linear1d;
  types["linear2d"]  = BDSInterpolatorType::linear2d;
  types["linear3d"]  = BDSInterpolatorType::linear3d;
  types["linear4d"]  = BDSInterpolatorType::linear4d;
  types["cubic1d"]   = BDSInterpolatorType::cubic1d;
  types["cubic2d"]   = BDSInterpolatorType::cubic2d;
  types["cubic3d"]   = BDSInterpolatorType::cubic3d;
  types["cubic4d"]   = BDSInterpolatorType::cubic4d;

  interpolatorType.toLower();

  auto result = types.find(interpolatorType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << "\"" << interpolatorType
	     << "\" is not a valid field type" << G4endl;

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
