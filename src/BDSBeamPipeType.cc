#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include <map>

// dictionary for BDSBeamPipeType
template<>
std::map<BDSBeamPipeType, std::string>* BDSBeamPipeType::dictionary =
  new std::map<BDSBeamPipeType, std::string> ({
   {BDSBeamPipeType::circular,       "circular"},
   {BDSBeamPipeType::elliptical,     "elliptical"},
   {BDSBeamPipeType::lhc,            "lhc"},
   {BDSBeamPipeType::lhcdetailed,    "lhcdetailed"},
   {BDSBeamPipeType::rectangular,    "rectangular"},
   {BDSBeamPipeType::rectellipse,    "rectellipse"},
   {BDSBeamPipeType::racetrack,      "racetrack"},
   {BDSBeamPipeType::octagonal,      "octagonal"},
   {BDSBeamPipeType::circularvacuum, "circularvacuum"}
});	

BDSBeamPipeType BDS::DetermineBeamPipeType(G4String apertureType)
{
  std::map<G4String, BDSBeamPipeType> types;
  types["circular"]       = BDSBeamPipeType::circular;
  types["elliptical"]     = BDSBeamPipeType::elliptical;
  types["rectangular"]    = BDSBeamPipeType::rectangular;
  types["lhc"]            = BDSBeamPipeType::lhc;
  types["lhcscreen"]      = BDSBeamPipeType::lhc; // shortcut for madx compatability
  types["lhcdetailed"]    = BDSBeamPipeType::lhcdetailed;
  types["rectellipse"]    = BDSBeamPipeType::rectellipse;
  types["racetrack"]      = BDSBeamPipeType::racetrack;
  types["octagonal"]      = BDSBeamPipeType::octagonal;
  types["circularvacuum"] = BDSBeamPipeType::circularvacuum;

  apertureType.toLower();

  auto result = types.find(apertureType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << apertureType << " is not a valid apertureType" << G4endl;

      G4cout << "Available geometry types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined aperture type to be " << result->second  << G4endl;
#endif
  return result->second;
}
