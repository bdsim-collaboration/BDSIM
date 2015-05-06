#include "BDSTunnelType.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "globals.hh"

#include <map>

BDSTunnelType BDS::DetermineTunnelType(G4String tunnelType, G4bool globalCheck)
{
  std::map<G4String, BDSTunnelType> types;
  types["circular"]               = BDSTunnelType::circular;
  types["elliptical"]             = BDSTunnelType::elliptical;
  types["square"]                 = BDSTunnelType::square;
  types["rectangular"]            = BDSTunnelType::rectangular;
  types["ilc"]                    = BDSTunnelType::ilc;
  types["rectangularaboveground"] = BDSTunnelType::rectangularaboveground;

  G4String tunnelTypeLower = G4String(tunnelType); // copy if first
  tunnelTypeLower.toLower();
    
  if ( (types.find(tunnelType) == types.end()) && (types.find(tunnelTypeLower) == types.end()) )
    {
      if (globalCheck){
	// it's not a valid key
	G4cerr << __METHOD_NAME__ << " " << tunnelType << " is not a valid tunnel type" << G4endl;
	exit(1);
      }
      else {
	// normal check - we can use BDSGlobalConstants - can't do this above as BDSGlobalConstants is using this function!
	return BDSGlobalConstants::Instance()->GetTunnelInfo().tunnelType;
      }
    }

  BDSTunnelType returnValue = types[tunnelType];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined tunnel type to be " << returnValue << G4endl;
#endif
  return returnValue;
}
