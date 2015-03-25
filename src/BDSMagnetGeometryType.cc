#include "BDSMagnetGeometryType.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "globals.hh"

#include <map>

BDSMagnetGeometryType BDS::DetermineMagnetGeometryType(G4String geometryType, G4bool globalCheck)
{
  std::map<G4String, BDSMagnetGeometryType> types;
  types["cylindrical"]       = BDSMagnetGeometryType::cylindrical;
  types["normalconducting"]  = BDSMagnetGeometryType::normalconducting;
  types["lhcleft"]           = BDSMagnetGeometryType::lhcleft;
  types["lhcright"]          = BDSMagnetGeometryType::lhcright;
  
  if (types.find(geometryType) == types.end())
    {
      if (globalCheck){
	// it's not a valid key
	G4cout << __METHOD_NAME__ << "\"" << geometryType << "\" is not a valid geometry type" << G4endl;
	exit(1);
      }
      else {
	// normal check - we can use BDSGlobalConstants - can't do this above as BDSGlobalConstants is using this function!
	return BDSGlobalConstants::Instance()->GetMagnetGeometryType();
      }
    }

  BDSMagnetGeometryType returnValue = types[geometryType];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined geometry type to be " << returnValue << G4endl;
#endif
  return returnValue;
}
