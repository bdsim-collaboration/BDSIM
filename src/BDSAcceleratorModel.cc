#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

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
  flatBeamline(NULL),
  supportsBeamline(NULL),
  tunnelBeamline(NULL)
{
  BDSAcceleratorComponentRegistry::Instance();
  BDSPhysicalVolumeInfoRegistry::Instance();
}

BDSAcceleratorModel::~BDSAcceleratorModel()
{
  delete worldPV;
  delete readOutWorldPV;
  delete readOutWorldLV;
  delete flatBeamline;
  delete supportsBeamline;
  delete tunnelBeamline;
  delete BDSAcceleratorComponentRegistry::Instance();
  delete BDSPhysicalVolumeInfoRegistry::Instance();

  _instance = 0;
}
