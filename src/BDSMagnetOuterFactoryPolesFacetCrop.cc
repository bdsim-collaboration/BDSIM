#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"

#include "globals.hh"  // geant4 globals / types

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
