#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "G4ProductionCuts.hh"
#include "G4Region.hh"

#include <map>

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
  endPieceBeamline(nullptr),
  placementBeamline(nullptr)
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

  for (auto r : regions)
    {delete r.second;}
  for (auto c : cuts)
    {delete c.second;}
  
  _instance = nullptr;
}

void BDSAcceleratorModel::RegisterRegion(G4Region* region, G4ProductionCuts* cut)
{
  G4String name = region->GetName();
  regions[name] = region;
  cuts[name]    = cut;
}

G4Region* BDSAcceleratorModel::Region(G4String name) const
{
  auto result = regions.find(name);
  if (result != regions.end())
    {return result->second;}
  else
    {
      G4cerr << "Invalid region name \"" << name << "\"" << G4endl;
      G4cout << "Available regions are: " << G4endl;
      for (const auto& r : regions)
	{G4cout << r.first << " ";}
      G4cout << G4endl;
      exit(1);
    }
}
