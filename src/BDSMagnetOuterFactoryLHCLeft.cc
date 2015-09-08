#include "BDSMagnetOuterFactoryLHCLeft.hh"
#include "BDSMagnetOuterFactoryLHC.hh"

BDSMagnetOuterFactoryLHCLeft* BDSMagnetOuterFactoryLHCLeft::_instance = nullptr;

BDSMagnetOuterFactoryLHCLeft* BDSMagnetOuterFactoryLHCLeft::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryLHCLeft();}
  return _instance;
}

BDSMagnetOuterFactoryLHCLeft::BDSMagnetOuterFactoryLHCLeft():
  BDSMagnetOuterFactoryLHC(true)
{
}

BDSMagnetOuterFactoryLHCLeft::~BDSMagnetOuterFactoryLHCLeft()
{
  _instance = nullptr;
}
