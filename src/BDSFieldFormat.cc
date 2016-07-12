#include "BDSFieldFormat.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>
#include <string>

// dictionary for BDSFieldFormat for reflexivity
template<>
std::map<BDSFieldFormat, std::string>* BDSFieldFormat::dictionary =
  new std::map<BDSFieldFormat, std::string> ({
      {BDSFieldFormat::none,      "none"},
      {BDSFieldFormat::bdsim2d,   "bdsim2d"},
      {BDSFieldFormat::bdsim3d,   "bdsim3d"}
});	

BDSFieldFormat BDS::DetermineFieldFormat(G4String bFormat)
{
  std::map<G4String, BDSFieldFormat> formats;
  formats["none"]    = BDSFieldFormat::none;
  formats["bdsim3d"] = BDSFieldFormat::bdsim2d;
  formats["bdsim3d"] = BDSFieldFormat::bdsim3d;
  
  bFormat.toLower();

  auto result = formats.find(bFormat);
  if (result == formats.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << bFormat << " is not a valid field format" << G4endl;

      G4cout << "Available field formats are:" << G4endl;
      for (auto it : formats)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined field format to be " << result->second << G4endl;
#endif
  return result->second;
}
