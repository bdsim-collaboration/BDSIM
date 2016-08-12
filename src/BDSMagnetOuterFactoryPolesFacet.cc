#include "BDSMagnetOuterFactoryPolesFacet.hh"

BDSMagnetOuterFactoryPolesFacet* BDSMagnetOuterFactoryPolesFacet::_instance = nullptr;

BDSMagnetOuterFactoryPolesFacet* BDSMagnetOuterFactoryPolesFacet::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesFacet();}
  return _instance;
}

BDSMagnetOuterFactoryPolesFacet::BDSMagnetOuterFactoryPolesFacet():
  BDSMagnetOuterFactoryPolesFacetCommon(1)
{;}

BDSMagnetOuterFactoryPolesFacet::~BDSMagnetOuterFactoryPolesFacet()
{
  _instance = nullptr;
}
