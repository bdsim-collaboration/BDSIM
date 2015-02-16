#include "BDSBeamPipeType.hh"


BDSBeamPipeType BDS::DetermineBeamPipeType(G4String apertureType)
{
  if( !strcmp(apertureType , "circular") )
    {return BDSBeamPipeType::circular;}
  else if ( !strcmp(apertureType, "elliptical") )
    {return BDSBeamPipeType::elliptical;}
  else if ( !strcmp(apertureType, "rectanglular") )
    {return BDSBeamPipeType::rectangular;}

  else
    {return BDSBeamPipeType::circular;} // default case
}

BDSBeamPipeType BDS::DetermineBeamPipeType(Element& element)
{
  return DetermineBeamPipeType(element.apertureType);
}
