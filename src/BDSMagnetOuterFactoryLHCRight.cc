#include "BDSMagnetOuterFactoryLHCRight.hh"
#include "BDSMagnetOuterFactoryLHC.hh"

//#include "BDSDebug.hh"

BDSMagnetOuterFactoryLHCRight* BDSMagnetOuterFactoryLHCRight::_instance = 0;

BDSMagnetOuterFactoryLHCRight* BDSMagnetOuterFactoryLHCRight::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactoryLHCRight();}
  return _instance;
}

BDSMagnetOuterFactoryLHCRight::BDSMagnetOuterFactoryLHCRight():
  BDSMagnetOuterFactoryLHC()
{
  isLeftOffset   = false;
}

BDSMagnetOuterFactoryLHCRight::~BDSMagnetOuterFactoryLHCRight()
{
  _instance = 0;
}
