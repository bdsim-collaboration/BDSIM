#include "BDSBType.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>

// dictionary for BDSBType
template<>
std::map<BDSBType, std::string>* BDSBType::dictionary =
  new std::map<BDSBType, std::string> ({
      {BDSBType::zero, "zero"},
      {BDSBType::threed, "threed"},
      {BDSBType::xy,     "xy"},
      {BDSBType::mokka,  "mokka"},
});	

BDSBType BDS::DetermineBType(G4String bType)
{
  std::map<G4String, BDSBType> types;
  types["zero"]   = BDSBType::zero;
  types["threed"] = BDSBType::threed;
  types["xy"]     = BDSBType::xy;
  types["mokka"]  = BDSBType::mokka;

  bType.toLower();

  auto result = types.find(bType);
  if ( result == types.end() )
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << " " << bType << " is not a valid B field type" << G4endl;
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined B field type to be " << result->second << G4endl;
#endif
  return result->second;
}
