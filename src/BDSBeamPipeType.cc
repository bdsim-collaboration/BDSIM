#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"
#include "globals.hh"

BDSBeamPipeType BDS::DetermineBeamPipeType(G4String apertureType)
{
  G4cout << apertureType << G4endl;
  if( apertureType.compare("circular") == 0 )
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " -> circular beampipe" << G4endl;
#endif
      return BDSBeamPipeType::circular;
    }
  else if ( apertureType.compare("elliptical") == 0 )
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " -> elliptical beampipe" << G4endl;
#endif
      return BDSBeamPipeType::elliptical;
    }
  else if ( apertureType.compare("rectangular") == 0 )
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " -> rectangular beampipe" << G4endl;
#endif
      return BDSBeamPipeType::rectangular;
    }
  else
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " -> Unknown beampipe type " << apertureType  << " making a circular beampipe" << G4endl;
#endif
      return BDSBeamPipeType::circular;
    } // default case
}

BDSBeamPipeType BDS::DetermineBeamPipeType(Element& element)
{
  return DetermineBeamPipeType(element.apertureType);
}
