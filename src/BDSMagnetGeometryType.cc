#include "BDSMagnetGeometryType.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>

BDSMagnetGeometryType BDS::DetermineMagnetGeometryType(G4String geometryType)
{
  std::map<G4String, BDSMagnetGeometryType> types;
  types["cylindrical"]       = BDSMagnetGeometryType::cylindrical;
  types["polescircular"]     = BDSMagnetGeometryType::polescircular;
  types["polessquare"]       = BDSMagnetGeometryType::polessquare;
  types["polesfacet"]        = BDSMagnetGeometryType::polesfacet;
  types["polesfacetcrop"]    = BDSMagnetGeometryType::polesfacetcrop;
  types["lhcleft"]           = BDSMagnetGeometryType::lhcleft;
  types["lhcright"]          = BDSMagnetGeometryType::lhcright;

  geometryType.toLower();
  
  if (types.find(geometryType) == types.end())
    {
      // it's not a valid key
      G4cout << __METHOD_NAME__ << "\"" << geometryType << "\" is not a valid geometry type" << G4endl;
      exit(1);
    }

  BDSMagnetGeometryType returnValue = types[geometryType];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined geometry type to be " << returnValue << G4endl;
#endif
  return returnValue;
}
