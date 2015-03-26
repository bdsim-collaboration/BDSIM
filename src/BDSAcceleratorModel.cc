#include "BDSAcceleratorModel.hh"

BDSAcceleratorModel* BDSAcceleratorModel::_instance = 0;

BDSAcceleratorModel* BDSAcceleratorModel::Instance()
{
  if (_instance == 0)
    {
      _instance = new BDSAcceleratorModel();
    }
  return _instance;
}


BDSAcceleratorModel::BDSAcceleratorModel():
  readOutWorld(NULL)
{;}
