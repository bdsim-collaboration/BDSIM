#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

BDSAcceleratorModel* BDSAcceleratorModel::_instance = nullptr;

BDSAcceleratorModel* BDSAcceleratorModel::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSAcceleratorModel();}
  return _instance;
}

BDSAcceleratorModel::BDSAcceleratorModel():
  worldPV(nullptr),
  flatBeamline(nullptr),
  curvilinearBeamline(nullptr),
  supportsBeamline(nullptr),
  tunnelBeamline(nullptr),
  endPieceBeamline(nullptr)
{
  BDSAcceleratorComponentRegistry::Instance();
  BDSPhysicalVolumeInfoRegistry::Instance();
}

BDSAcceleratorModel::~BDSAcceleratorModel()
{
  delete worldPV;
  delete flatBeamline;
  delete curvilinearBeamline;
  delete supportsBeamline;
  delete tunnelBeamline;
  delete endPieceBeamline;
  delete BDSAcceleratorComponentRegistry::Instance();
  delete BDSPhysicalVolumeInfoRegistry::Instance();

  _instance = nullptr;
}
