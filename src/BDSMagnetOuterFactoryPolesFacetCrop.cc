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
