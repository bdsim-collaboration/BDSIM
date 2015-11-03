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
   {BDSBeamPipeType::racetrack,  "racetrack"}
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

void BDS::CheckApertureInfo(BDSBeamPipeInfo*& info)
{
  CheckApertureInfo(info->beamPipeType,
		    info->aper1,
		    info->aper2,
		    info->aper3,
		    info->aper4);
}

void BDS::CheckApertureInfo(BDSBeamPipeType beamPipeTypeIn,
			    G4double& aper1,
			    G4double& aper2,
			    G4double& aper3,
			    G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (beamPipeTypeIn == BDSBeamPipeType::circular)
    {InfoOKForCircular(aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::elliptical)
    {InfoOKForElliptical(aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::rectangular)
    {InfoOKForRectangular(aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::lhc)
    {InfoOKForLHC(aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::lhcdetailed)
    {InfoOKForLHCDetailed(aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::rectellipse)
    {InfoOKForRectEllipse(aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::racetrack)
    {InfoOKForRaceTrack(aper1,aper2,aper3,aper4);}
  else
    {InfoOKForCircular(aper1,aper2,aper3,aper4);}
}

void BDS::CheckRequiredParametersSet(G4double& aper1, G4bool setAper1,
				     G4double& aper2, G4bool setAper2,
				     G4double& aper3, G4bool setAper3,
				     G4double& aper4, G4bool setAper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "aper1: " << aper1 << " check it? " << setAper1 << G4endl;
  G4cout << "aper2: " << aper2 << " check it? " << setAper2 << G4endl;
  G4cout << "aper3: " << aper3 << " check it? " << setAper3 << G4endl;
  G4cout << "aper4: " << aper4 << " check it? " << setAper4 << G4endl;
#endif
  G4bool shouldExit = false;
  if (setAper1)
    {
      if (!BDS::IsFinite(aper1))
	{G4cerr << "\"aper1\" not set, but required to be" << G4endl; shouldExit = true;}
    }

  if (setAper2)
    {
      if (!BDS::IsFinite(aper2))
	{G4cerr << "\"aper2\" not set, but required to be" << G4endl; shouldExit = true;}
    }

  if (setAper3)
    {
      if (!BDS::IsFinite(aper3))
	{G4cerr << "\"aper3\" not set, but required to be" << G4endl; shouldExit = true;}
    }

  if (setAper4)
    {
      if (!BDS::IsFinite(aper4))
	{G4cerr << "\"aper4\" not set, but required to be" << G4endl; shouldExit = true;}
    }

  if (shouldExit)
    {exit(1);}
}

void BDS::InfoOKForCircular(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CheckRequiredParametersSet(aper1, true, aper2, false, aper3, false, aper4, false);
}

void BDS::InfoOKForElliptical(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CheckRequiredParametersSet(aper1, true, aper2, true, aper3, false, aper4, false);
}

void BDS::InfoOKForRectangular(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CheckRequiredParametersSet(aper1, true, aper2, true, aper3, false, aper4, false);
}

void BDS::InfoOKForLHC(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CheckRequiredParametersSet(aper1, true, aper2, true, aper3, true, aper4, false);

  if ((aper3 > aper1) and (aper2 < aper3))
    {
      G4cerr << __METHOD_NAME__ << "WARNING - \"aper3\" > \"aper1\" (or \"beamPipeRadius\") for lhc aperture model - will not produce desired shape" << G4endl;
      exit(1);
    }

  if ((aper3 > aper2) and (aper1 < aper3))
    {
      G4cerr << __METHOD_NAME__ << "WARNING - \"aper3\" > \"aper2\" (or \"beamPipeRadius\") for lhc aperture model - will not produce desired shape" << G4endl;
      exit(1);
    }
}

void BDS::InfoOKForLHCDetailed(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  InfoOKForLHC(aper1,aper2,aper3,aper4);
}

void BDS::InfoOKForRectEllipse(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CheckRequiredParametersSet(aper1, true, aper2, true, aper3, true, aper4, true);

  // TBC
  
  /*
  //treat rectangle as point coordinates.  If this point is inside ellipse,
  //rectangle is too small -> error should just use elliptical aperture
  G4double test = sqrt((aper1/aper3)*(aper1/aper3) + (aper2/aper4)*(aper2/aper4));
  if (test < 1)
    {
      //rectangle too small
      G4cerr << __METHOD_NAME__ << "WARNING - combination of \"aper1\" and \"aper2\" too small"
	     << " rectangle is entirely inside ellipse - simply use elliptical aperture" << G4endl;
      exit(1);
    }
  */
  //check if ellipse is b
  if ((aper1 > aper3) and (aper2 > aper4))
    {;}
}

void BDS::InfoOKForRaceTrack(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CheckRequiredParametersSet(aper1, true, aper2, true, aper3, true, aper4, false);
}
