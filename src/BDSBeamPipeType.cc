#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>

BDSBeamPipeType BDS::DetermineBeamPipeType(G4String apertureType)
{
  std::map<G4String, BDSBeamPipeType> types;
  types["circular"]    = BDSBeamPipeType::circular;
  types["elliptical"]  = BDSBeamPipeType::elliptical;
  types["rectangular"] = BDSBeamPipeType::rectangular;
  types["lhc"]         = BDSBeamPipeType::lhc;
  types["lhcdetailed"] = BDSBeamPipeType::lhcdetailed;
  types["rectellipse"] = BDSBeamPipeType::rectellipse;

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

void BDS::CheckApertureInfo(BDSBeamPipeType beamPipeTypeIn, G4double& beamPipeRadius,
			    G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
  if (beamPipeTypeIn == BDSBeamPipeType::circular)
    {InfoOKForCircular(beamPipeRadius,aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::elliptical)
    {InfoOKForElliptical(beamPipeRadius,aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::rectangular)
    {InfoOKForRectangular(beamPipeRadius,aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::lhc)
    {InfoOKForLHC(beamPipeRadius,aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::lhcdetailed)
    {InfoOKForLHCDetailed(beamPipeRadius,aper1,aper2,aper3,aper4);}
  if (beamPipeTypeIn == BDSBeamPipeType::rectellipse)
    {InfoOKForRectEllipse(beamPipeRadius,aper1,aper2,aper3,aper4);}
  else
    {InfoOKForCircular(beamPipeRadius,aper1,aper2,aper3,aper4);}
}

void BDS::InfoOKForCircular(G4double& beamPipeRadius, G4double& aper1, G4double& /*aper2*/, G4double& /*aper3*/, G4double& /*aper4*/)
{
  if ((beamPipeRadius == 0) && (aper1 == 0)) {
      G4cerr << __METHOD_NAME__ << "Error: option \"beampipeRadius\" or \"aper1\" must be greater than 0 for all aperture types" << G4endl;
      exit(1);
    }
  if ((aper1 == 0) && (beamPipeRadius > 0)) {
      // beampiperadius set but aper1 not - need aper1 to be set - copy value from beampipe radius
      aper1 = beamPipeRadius;
    }
  if ((beamPipeRadius == 0) && (aper1 > 0)) {
      // aper1 set but beampiperadius not - copy just in case
      beamPipeRadius = aper1;
    }
}

void BDS::InfoOKForElliptical(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
  // can actually use the same checks for rectangular as two parameter
  InfoOKForRectangular(beamPipeRadius,aper1,aper2,aper3,aper4);
}

void BDS::InfoOKForRectangular(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
  // basic circular checks first - require at least one parameter for a square beam pipe
  InfoOKForCircular(beamPipeRadius,aper1,aper2,aper3,aper4);

  if ((aper2 == 0) && (aper1 > 0)) {
    // note if aper1 is > 0 so must beampipe radius or they share a value from circular checks
    G4cerr << __METHOD_NAME__ << "WARNING - \"aper2\" not set for rectangular aperture model" << G4endl;
    exit(1);
    //aper2 = aper1;
  }

  if ((aper2 > 0) && (aper1 == 0)) {
    // aper2 set but not aper1 - copy it to aper1
    G4cerr << __METHOD_NAME__ << "WARNING - \"aper2\" set but \"aper1\" not set for rectangular aperture model" << G4endl;
    exit(1);
    //aper1 = aper2;
  }
}

void BDS::InfoOKForLHC(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
  InfoOKForCircular(beamPipeRadius,aper1,aper2,aper3,aper4);

  if (aper2 == 0) {
    // aper2 isn't set
    G4cerr << __METHOD_NAME__ << "WARNING - \"aper2\" not set for lhc aperture model" << G4endl;
    exit(1);
  }

  if (aper3 == 0) {
    // aper3 isn't set
    G4cerr << __METHOD_NAME__ << "WARNING - \"aper3\" not set for lhc aperture model" << G4endl;
    exit(1);
  }

  if ((aper3 > aper1) and (aper2 < aper3)) {
    G4cerr << __METHOD_NAME__ << "WARNING - \"aper3\" > \"aper1\" (or \"beamPipeRadius\") for lhc aperture model - will not produce desired shape" << G4endl;
    exit(1);
  }

  if ((aper3 > aper2) and (aper1 < aper3)) {
    G4cerr << __METHOD_NAME__ << "WARNING - \"aper3\" > \"aper2\" (or \"beamPipeRadius\") for lhc aperture model - will not produce desired shape" << G4endl;
    exit(1);
  }
}

void BDS::InfoOKForLHCDetailed(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
  InfoOKForLHC(beamPipeRadius,aper1,aper2,aper3,aper4);
}

void BDS::InfoOKForRectEllipse(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4)
{
  // basic checks
  InfoOKForCircular(beamPipeRadius,aper1,aper2,aper3,aper4);
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
