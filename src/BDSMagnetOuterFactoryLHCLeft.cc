#include "BDSMagnetOuterFactoryLHCLeft.hh"
#include "BDSMagnetOuterFactoryLHC.hh"

//#include "BDSDebug.hh"

BDSMagnetOuterFactoryLHCLeft* BDSMagnetOuterFactoryLHCLeft::_instance = 0;

BDSMagnetOuterFactoryLHCLeft* BDSMagnetOuterFactoryLHCLeft::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactoryLHCLeft();}
  return _instance;
}

BDSMagnetOuterFactoryLHCLeft::BDSMagnetOuterFactoryLHCLeft():
  BDSMagnetOuterFactoryLHC()
{
  isLeftOffset   = true;
}

BDSMagnetOuterFactoryLHCLeft::~BDSMagnetOuterFactoryLHCLeft()
{
  _instance = 0;
}
