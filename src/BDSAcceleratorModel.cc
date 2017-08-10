#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSFieldObjects.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

#include <cstdio>
#include <map>

BDSAcceleratorModel* BDSAcceleratorModel::instance = nullptr;

BDSAcceleratorModel* BDSAcceleratorModel::Instance()
{
  if (!instance)
    {instance = new BDSAcceleratorModel();}
  return instance;
}

BDSAcceleratorModel::BDSAcceleratorModel():
  worldPV(nullptr),
  worldLV(nullptr),
  worldSolid(nullptr),
  flatBeamline(nullptr),
  curvilinearBeamline(nullptr),
  supportsBeamline(nullptr),
  tunnelBeamline(nullptr),
  endPieceBeamline(nullptr),
  placementBeamline(nullptr)
{
  removeTemporaryFiles = BDSGlobalConstants::Instance()->RemoveTemporaryFiles();
  BDSAcceleratorComponentRegistry::Instance();
  BDSPhysicalVolumeInfoRegistry::Instance();
}

BDSAcceleratorModel::~BDSAcceleratorModel()
{
  // User feedback as deletion can take some time
  G4cout << "BDSAcceleratorModel> Deleting model" << G4endl;
  
  delete worldPV;
  delete worldLV;
  delete worldSolid;
  delete flatBeamline;
  delete curvilinearBeamline;
  delete supportsBeamline;
  delete tunnelBeamline;
  delete endPieceBeamline;
  delete placementBeamline;
  delete BDSAcceleratorComponentRegistry::Instance();
  delete BDSPhysicalVolumeInfoRegistry::Instance();

  for (auto f : fields)
    {delete f;}
  for (auto r : regions)
    {delete r.second;}
  for (auto c : cuts)
    {delete c.second;}

  G4cout << "BDSAcceleratorModel> Deletion complete" << G4endl;

  if (removeTemporaryFiles)
    {
      G4cout << "BDSAcceleratorModel> Removing temporary files" << G4endl;
      for (auto& filename : temporaryFiles)
	{
	  G4cout << "Removing \"" << filename << "\"" << G4endl;
	  std::remove(filename);
	}
      G4cout << "BDSAcceleratorModel> Temporary files removed" << G4endl;
    }
  instance = nullptr;
}

void BDSAcceleratorModel::RegisterRegion(G4Region* region, G4ProductionCuts* cut)
{
  G4String name = region->GetName();
  regions[name] = region;
  cuts[name]    = cut;
}

void BDSAcceleratorModel::RegisterTemporaryFile(G4String fileName)
{
  temporaryFiles.push_back(fileName);
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
