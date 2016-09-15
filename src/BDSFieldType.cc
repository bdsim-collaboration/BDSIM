#include "BDSFieldType.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>
#include <string>

// dictionary for BDSFieldType for reflexivity
template<>
std::map<BDSFieldType, std::string>* BDSFieldType::dictionary =
  new std::map<BDSFieldType, std::string> ({
      {BDSFieldType::none,             "none"},
      {BDSFieldType::zero,             "zero"},
      {BDSFieldType::threed,           "threed"},
      {BDSFieldType::xy,               "xy"},
      {BDSFieldType::mokka,            "mokka"},
      {BDSFieldType::solenoid,         "solenoid"},
      {BDSFieldType::dipole,           "dipole"},
      {BDSFieldType::quadrupole,       "quadrupole"},
      {BDSFieldType::dipolequadrupole, "dipolequadrupole"},
      {BDSFieldType::sextupole,        "sextupole"},
      {BDSFieldType::octupole,         "octupole"},
      {BDSFieldType::decapole,         "decapole"},
      {BDSFieldType::multipole,        "multipole"},
      {BDSFieldType::fringe,           "fringe"},
      {BDSFieldType::muonspoiler,      "muonspoiler"},
      {BDSFieldType::rfcavity,         "rfcavity"},
      {BDSFieldType::rf,               "rf"}
});	

BDSFieldType BDS::DetermineFieldType(G4String bType)
{
  std::map<G4String, BDSFieldType> types;
  types["none"]             = BDSFieldType::none;
  types["zero"]             = BDSFieldType::zero;
  types["threed"]           = BDSFieldType::threed;
  types["xy"]               = BDSFieldType::xy;
  types["mokka"]            = BDSFieldType::mokka;
  types["solenoid"]         = BDSFieldType::solenoid;
  types["dipole"]           = BDSFieldType::dipole;
  types["quadrupole"]       = BDSFieldType::quadrupole;
  types["dipolequadrupole"] = BDSFieldType::dipolequadrupole;
  types["sextupole"]        = BDSFieldType::sextupole;
  types["octupole"]         = BDSFieldType::octupole;
  types["decapole"]         = BDSFieldType::decapole;
  types["multipole"]        = BDSFieldType::multipole;
  types["fringe"]           = BDSFieldType::fringe;
  types["muonspoiler"]      = BDSFieldType::muonspoiler;
  types["rfcavity"]         = BDSFieldType::rfcavity;
  types["rf"]               = BDSFieldType::rf;
  
  bType.toLower();

  auto result = types.find(bType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << bType << " is not a valid field type" << G4endl;

      G4cout << "Available field types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined field type to be " << result->second << G4endl;
#endif
  return result->second;
}
