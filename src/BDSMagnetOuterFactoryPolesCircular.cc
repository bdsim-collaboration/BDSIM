#include "BDSMagnetOuterFactoryPolesCircular.hh"

BDSMagnetOuterFactoryPolesCircular* BDSMagnetOuterFactoryPolesCircular::_instance = nullptr;

BDSMagnetOuterFactoryPolesCircular* BDSMagnetOuterFactoryPolesCircular::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesCircular();}
  return _instance;
}

BDSMagnetOuterFactoryPolesCircular::~BDSMagnetOuterFactoryPolesCircular()
{
  _instance = nullptr;
}
