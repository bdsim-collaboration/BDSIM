#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

BDSAcceleratorModel* BDSAcceleratorModel::_instance = nullptr;

BDSAcceleratorModel* BDSAcceleratorModel::Instance()
{
  if (_instance == nullptr)
    {
      _instance = new BDSAcceleratorModel();
    }
  return _instance;
}


BDSAcceleratorModel::BDSAcceleratorModel():
  worldPV(nullptr),
  readOutWorldPV(nullptr),
  readOutWorldLV(nullptr),
  tunnelReadOutWorldPV(nullptr),
  tunnelReadOutWorldLV(nullptr),
  flatBeamline(nullptr),
  supportsBeamline(nullptr),
  tunnelBeamline(nullptr)
{
  BDSAcceleratorComponentRegistry::Instance();
  BDSPhysicalVolumeInfoRegistry::Instance();
}

BDSAcceleratorModel::~BDSAcceleratorModel()
{
  delete worldPV;          worldPV          = nullptr;
  delete readOutWorldPV;   readOutWorldPV   = nullptr;
  delete readOutWorldLV;   readOutWorldLV   = nullptr;
  delete flatBeamline;     flatBeamline     = nullptr;
  delete supportsBeamline; supportsBeamline = nullptr;
  delete tunnelBeamline;   tunnelBeamline   = nullptr;
  delete BDSAcceleratorComponentRegistry::Instance();
  delete BDSPhysicalVolumeInfoRegistry::Instance();

  _instance = nullptr;
}
