/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSImportanceFileLoader.hh"
#include "BDSMaterials.hh"
#include "BDSParallelWorldImportance.hh"
#include "BDSParser.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSSamplerType.hh"
#include "BDSSDManager.hh"
#include "BDSSurvey.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/options.h"
#include "parser/physicsbiasing.h"
#include "parser/placement.h"

#include "globals.hh"
#include "G4Box.hh"
#include "G4GeometryCell.hh"
#include "G4IStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSensitiveDetector.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <vector>

BDSParallelWorldImportance::BDSParallelWorldImportance(G4String name):
  G4VUserParallelWorld("importanceWorld_" + name)
{
  globals = BDSGlobalConstants::Instance();
  verbose       = globals->Verbose();
  checkOverlaps = globals->CheckOverlaps();
}

void BDSParallelWorldImportance::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(globals->ImportanceVolumeMapFile());
  imVolumesAndValues = BDSImportanceFileLoader::Instance()->Load(importanceMapFile);

  // build world
  BuildWorld();

  // create an importance store which adds importance values to cells
  AddIStore();
}

BDSParallelWorldImportance::~BDSParallelWorldImportance()
{;}

void BDSParallelWorldImportance::BuildWorld()
{
  // load the importance world geometry
  std::string geometryFile = globals->ImportanceWorldGeometryFile();
  geom = BDSGeometryFactory::Instance()->BuildGeometry("importanceWorld", geometryFile, nullptr, 0, 0);

  //clone mass world for parallel world PV
  imWorldPV = GetWorld();

  G4LogicalVolume *worldLV = imWorldPV->GetLogicalVolume();

  G4VisAttributes* samplerWorldVis = new G4VisAttributes(*(globals->VisibleDebugVisAttr()));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(samplerWorldVis);
  parallelLogicalVolumes.push_back(worldLV);

  // set limits
  worldLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  if (BDSGlobalConstants::Instance()->StoreELossWorld())
    {
      worldLV->SetSensitiveDetector(BDSSDManager::Instance()->GetWorldCompleteSD());
      geom->AttachSensitiveDetectors();
    }

  std::vector<G4LogicalVolume*> parallelLVs = geom->GetAllLogicalVolumes();
  std::vector<G4VPhysicalVolume*> parallelPVs = geom->GetAllPhysicalVolumes();

  // loop over all parallel physical and logical volumes, place appropriately and add to store.
  for (G4int i=0; i < (G4int)parallelPVs.size(); i++)
    {
        G4LogicalVolume* lv = parallelLVs[i+1];
        parallelLogicalVolumes.push_back(lv);

        G4VPhysicalVolume* pv = parallelPVs[i];

        auto name = pv->GetName();

        G4VPhysicalVolume *parallelPV = new G4PVPlacement(pv->GetRotation(),
                                            pv->GetTranslation(),
                                            lv,
                                            pv->GetName(),
                                            worldLV,
                                            false,
                                            0);
        // second arg is replica number - if we assume no replication of cells for now, i should be 0??
        G4GeometryCell cell(*parallelPV, 0);
        imVolumeStore.AddPVolume(cell);
    }
}

G4GeometryCell BDSParallelWorldImportance::GetGeometryCell(G4int i){
  const G4VPhysicalVolume *p = imVolumeStore.GetPVolume(i);
  if (p)
    {return G4GeometryCell(*p,0);}
  else {
    G4cout << __METHOD_NAME__ << G4endl
           << " couldn't get G4GeometryCell" << G4endl;
    return G4GeometryCell(*imWorldPV,-2);
  }
}

void BDSParallelWorldImportance::AddIStore() {
  G4IStore* aIstore = G4IStore::GetInstance(imWorldPV->GetName());

  // create a geometry cell for the world volume replicaNumber is 0!
  G4GeometryCell gWorldVolumeCell(*imWorldPV, 0);

  // set world volume importance to 1
  aIstore->AddImportanceGeometryCell(1, gWorldVolumeCell);

  // set importance values and create scorers
  G4int numCells = (G4int) imVolumesAndValues.size();

  for (G4int cell = 0; cell < numCells; cell++) {
    G4GeometryCell gCell  = GetGeometryCell(cell);
    G4String cellName     = gCell.GetPhysicalVolume().GetName();
    G4int importanceValue = GetCellImportanceValue(cellName);

    //only add to store if it hasn't already been added.
    if (!aIstore->IsKnown(gCell))
      {aIstore->AddImportanceGeometryCell(importanceValue, gCell.GetPhysicalVolume(), cell);}
  }
}

G4int BDSParallelWorldImportance::GetCellImportanceValue(G4String cellName)
  {
    //TODO: proper check that imVolumesAndValues contains the provided cellName

    // cast value to int.
    G4int importanceValue = (G4int)(imVolumesAndValues)[cellName];
    // cell value should be finite - assume value of 1 (global value).
    if (!BDS::IsFinite(importanceValue))
      {importanceValue = 1;}
    return importanceValue;
  }

void BDSParallelWorldImportance::ConstructSD()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Sensitive Detector Name
  G4String concreteSDname = "ConcreteSD";

  G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(concreteSDname);
  SDman->AddNewDetector(MFDet); // Register SD to SDManager

  G4String fltName,particleName;
  G4SDParticleFilter* neutronFilter = new G4SDParticleFilter(fltName="neutronFilter", particleName="neutron");

  MFDet->SetFilter(neutronFilter);

  // set parallel volumes as sensitive.
  for (auto it = parallelLogicalVolumes.begin(); it != parallelLogicalVolumes.end(); it++)
    {SetSensitiveDetector((*it)->GetName(), MFDet);}

  //TODO: Add scorers here if necessary. Follow recipe in G4 example B03ImportanceDetectorConstruction.
}