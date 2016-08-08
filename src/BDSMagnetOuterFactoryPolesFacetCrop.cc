#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"


BDSMagnetOuterFactoryPolesFacetCrop* BDSMagnetOuterFactoryPolesFacetCrop::_instance = nullptr;

BDSMagnetOuterFactoryPolesFacetCrop* BDSMagnetOuterFactoryPolesFacetCrop::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesFacetCrop();}
  return _instance;
}

BDSMagnetOuterFactoryPolesFacetCrop::BDSMagnetOuterFactoryPolesFacetCrop():
  BDSMagnetOuterFactoryPolesFacetCommon(2)
{;}

BDSMagnetOuterFactoryPolesFacetCrop::~BDSMagnetOuterFactoryPolesFacetCrop()
{
  _instance = nullptr;
}

void BDSMagnetOuterFactoryPolesFacetCrop::CalculateStartAngles()
{
  polyStartAngle = segmentAngle*0.25;
  poleIntersectionStartAngle = -segmentAngle*0.25 - CLHEP::halfpi;
}
