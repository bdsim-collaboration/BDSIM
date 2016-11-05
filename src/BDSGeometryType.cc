#include "BDSGeometryType.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>

// dictionary for BDSGeometryType
template<>
std::map<BDSGeometryType, std::string>* BDSGeometryType::dictionary =
  new std::map<BDSGeometryType, std::string> ({
   {BDSGeometryType::threed, "threed"},
   {BDSGeometryType::xy,     "xy"},
   {BDSGeometryType::mokka,  "mokka"},
   {BDSGeometryType::gmad,   "gmad"},
   {BDSGeometryType::gdml,   "gdml"}
});	

BDSGeometryType BDS::DetermineGeometryType(G4String geometryType)
{
  std::map<G4String, BDSGeometryType> types;
  types["threed"] = BDSGeometryType::threed;
  types["xy"]     = BDSGeometryType::xy;
  types["mokka"]  = BDSGeometryType::mokka;
  types["gmad"]   = BDSGeometryType::gmad;
  types["gdml"]   = BDSGeometryType::gdml;

  geometryType.toLower();

  auto result = types.find(geometryType);
  if ( result == types.end() )
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << " " << geometryType << " is not a valid geometry type" << G4endl;
      
      G4cout << "Available geometry types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined geometry type to be " << result->second << G4endl;
#endif
  return result->second;
}

