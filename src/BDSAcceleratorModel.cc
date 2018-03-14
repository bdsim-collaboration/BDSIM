/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineSet.hh"
#include "BDSDebug.hh"
#include "BDSFieldObjects.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

#include <cstdio>
#include <map>
#include <vector>

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
  tunnelBeamline(nullptr),
  placementBeamline(nullptr)
{
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
  
  delete tunnelBeamline;
  delete placementBeamline;

  mainBeamlineSet.DeleteContents();
  
  for (auto& bl : extraBeamlines)
    {bl.second.DeleteContents();}
  
  delete BDSAcceleratorComponentRegistry::Instance();
  delete BDSPhysicalVolumeInfoRegistry::Instance();

  for (auto f : fields)
    {delete f;}
  for (auto r : regions)
    {delete r.second;}
  for (auto c : cuts)
    {delete c.second;}

  G4cout << "BDSAcceleratorModel> Deletion complete" << G4endl;

  instance = nullptr;
}

void BDSAcceleratorModel::RegisterBeamlineSetExtra(G4String              name,
						   const BDSBeamlineSet& setIn)
{
  auto search = extraBeamlines.find(name);
  if (search != extraBeamlines.end()) // already exists!
    {search->second.DeleteContents();} // delete pre-existing one for replacement
  extraBeamlines[name] = setIn;
}

const BDSBeamlineSet& BDSAcceleratorModel::BeamlineSet(G4String name) const
{
  if (name == "main")
    {return mainBeamlineSet;}
  
  const auto search = extraBeamlines.find(name);
  if (search == extraBeamlines.end())
    {G4cerr << __METHOD_NAME__ << "No such beam line set \"" << name << "\"" << G4endl; exit(1);}
  else
    {return search->second;}
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
