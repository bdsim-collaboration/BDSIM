#include "BDSMagnetOuterFactoryLHCRight.hh"
#include "BDSMagnetOuterFactoryLHC.hh"

BDSMagnetOuterFactoryLHCRight* BDSMagnetOuterFactoryLHCRight::_instance = nullptr;

BDSMagnetOuterFactoryLHCRight* BDSMagnetOuterFactoryLHCRight::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryLHCRight();}
  return _instance;
}

BDSMagnetOuterFactoryLHCRight::BDSMagnetOuterFactoryLHCRight():
  BDSMagnetOuterFactoryLHC(false)
{
}

BDSMagnetOuterFactoryLHCRight::~BDSMagnetOuterFactoryLHCRight()
{
  _instance = nullptr;
}
