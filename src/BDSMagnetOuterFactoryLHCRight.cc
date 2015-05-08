#include "BDSMagnetOuterFactoryLHCRight.hh"
#include "BDSMagnetOuterFactoryLHC.hh"

BDSMagnetOuterFactoryLHCRight* BDSMagnetOuterFactoryLHCRight::_instance = 0;

BDSMagnetOuterFactoryLHCRight* BDSMagnetOuterFactoryLHCRight::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactoryLHCRight();}
  return _instance;
}

BDSMagnetOuterFactoryLHCRight::BDSMagnetOuterFactoryLHCRight():
  BDSMagnetOuterFactoryLHC(false)
{
}

BDSMagnetOuterFactoryLHCRight::~BDSMagnetOuterFactoryLHCRight()
{
  _instance = 0;
}
