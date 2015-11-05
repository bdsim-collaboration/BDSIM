#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

#include <map>

// dictionary for BDSBeamPipeType
template<>
std::map<BDSBeamPipeType, std::string>* BDSBeamPipeType::dictionary =
  new std::map<BDSBeamPipeType, std::string> ({
   {BDSBeamPipeType::circular,   "circular"},
   {BDSBeamPipeType::elliptical, "elliptical"},
   {BDSBeamPipeType::lhc,        "lhc"},
   {BDSBeamPipeType::lhcdetailed,"lhcdetailed"},
   {BDSBeamPipeType::rectangular,"rectangular"},
   {BDSBeamPipeType::rectellipse,"rectellipse"},
   {BDSBeamPipeType::racetrack,  "racetrack"},
   {BDSBeamPipeType::octagonal,  "octagonal"}
});	

BDSBeamPipeType BDS::DetermineBeamPipeType(G4String apertureType)
{
  std::map<G4String, BDSBeamPipeType> types;
  types["circular"]    = BDSBeamPipeType::circular;
  types["elliptical"]  = BDSBeamPipeType::elliptical;
  types["rectangular"] = BDSBeamPipeType::rectangular;
  types["lhc"]         = BDSBeamPipeType::lhc;
  types["lhcdetailed"] = BDSBeamPipeType::lhcdetailed;
  types["rectellipse"] = BDSBeamPipeType::rectellipse;
  types["racetrack"]   = BDSBeamPipeType::racetrack;
  types["octagonal"]   = BDSBeamPipeType::octagonal;

  apertureType.toLower();

  if ( types.find(apertureType) == types.end() )
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << " " << apertureType << " is not a valid apertureType" << G4endl;
      exit(1);
    }

  BDSBeamPipeType returnValue = types[apertureType];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined aperture type to be " << returnValue << G4endl;
#endif
  return returnValue;
}
