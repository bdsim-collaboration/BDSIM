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
  worldPV(NULL),
  readOutWorldPV(NULL),
  readOutWorldLV(NULL),
  flatBeamline(NULL)
{;}

BDSAcceleratorModel::~BDSAcceleratorModel()
{
  delete worldPV;
  delete readOutWorldPV;
  delete readOutWorldLV;
  delete flatBeamline;

  _instance = 0;
}
