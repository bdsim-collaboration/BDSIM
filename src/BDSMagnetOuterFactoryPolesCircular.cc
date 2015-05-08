#include "BDSMagnetOuterFactoryPolesCircular.hh"

BDSMagnetOuterFactoryPolesCircular* BDSMagnetOuterFactoryPolesCircular::_instance = 0;

BDSMagnetOuterFactoryPolesCircular* BDSMagnetOuterFactoryPolesCircular::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactoryPolesCircular();}
  return _instance;
}

BDSMagnetOuterFactoryPolesCircular::~BDSMagnetOuterFactoryPolesCircular()
{
  _instance = 0;
}
