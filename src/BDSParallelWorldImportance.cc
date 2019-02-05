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
}

void BDSParallelWorldImportance::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(globals->ImportanceVolumeMapFile());
  BDSImportanceFileLoader importanceLoader;
  imVolumesAndValues = importanceLoader.Load(importanceMapFile);

  // build world
  BuildWorld();
}

BDSParallelWorldImportance::~BDSParallelWorldImportance()
{;}

void BDSParallelWorldImportance::BuildWorld()
{
  // load the importance world geometry
  std::string geometryFile = globals->ImportanceWorldGeometryFile();
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry("importanceWorld", geometryFile, nullptr, 0, 0);

  // clone mass world for parallel world PV
  imWorldPV = GetWorld();

  G4LogicalVolume *worldLV = imWorldPV->GetLogicalVolume();

  // set parallel world vis attributes
  G4VisAttributes* samplerWorldVis = new G4VisAttributes(*(globals->VisibleDebugVisAttr()));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(samplerWorldVis);
  parallelLogicalVolumes.push_back(worldLV);

  // set limits
  worldLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  // vectors of all logical and physical volumes to save accessing from geometry in for loop below.
  std::vector<G4LogicalVolume*> parallelLVs = geom->GetAllLogicalVolumes();
  std::vector<G4VPhysicalVolume*> parallelPVs = geom->GetAllPhysicalVolumes();

  // loop over all parallel physical and logical volumes, place appropriately and add to store.
  for (G4int i=0; i < (G4int)parallelPVs.size(); i++)
    {
      G4LogicalVolume* lv = parallelLVs[i+1];
      parallelLogicalVolumes.push_back(lv);

      G4VPhysicalVolume* pv = parallelPVs[i];

      G4VPhysicalVolume *parallelPV = new G4PVPlacement(pv->GetRotation(),
                                            pv->GetTranslation(),
                                            lv,
                                            pv->GetName(),
                                            worldLV,
                                            false,
                                            0);
      // second arg (0) is replica number - assume no replication of cells
      G4GeometryCell cell(*parallelPV, 0);
      imVolumeStore.AddPVolume(cell);
    }
}

G4VPhysicalVolume* BDSParallelWorldImportance::GetWorldVolume()
  {return imWorldPV;}


G4GeometryCell BDSParallelWorldImportance::GetGeometryCell(G4int i){
  const G4VPhysicalVolume *p = imVolumeStore.GetPVolume(i);
  if (p)
    {return G4GeometryCell(*p,0);}
  else {
    G4cout << __METHOD_NAME__ << " Couldn't get G4GeometryCell" << G4endl;
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
    G4double importanceValue = GetCellImportanceValue(cellName);

    //only add to store if it hasn't already been added. 0 is replica index.
    if (!aIstore->IsKnown(gCell))
      {aIstore->AddImportanceGeometryCell(importanceValue, gCell.GetPhysicalVolume(), 0);}
    else
      {
        G4cerr << __METHOD_NAME__ << "Geometry cell " << cellName << "already exists and has been previously "
               << "added to the IStore." << G4endl;
        exit(1);
      }
  }
}

G4double BDSParallelWorldImportance::GetCellImportanceValue(G4String cellName)
  {
    // set importance value to 1 if trying to get the value for a PV that isnt provided by the user.
    // cell value of 1 is global default
    G4double importanceValue = (imVolumesAndValues)[cellName];
    if (!BDS::IsFinite(importanceValue))
      {
        importanceValue = 1;
        //warning: importance value for parallel world volume << pV << has not been supplied. Setting value to one.
        G4cout << "" << G4endl;
      }
    return importanceValue;
  }

void BDSParallelWorldImportance::ConstructSD()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Sensitive Detector Name
  G4String importanceSamplingSDname = "ImportanceSamplingSD";

  // Create multifunctional detector for parallel world
  G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(importanceSamplingSDname);
  SDman->AddNewDetector(MFDet); // Register SD to SDManager

  // Neutron filter
  G4SDParticleFilter* neutronFilter = new G4SDParticleFilter("neutronFilter", "neutron");
  MFDet->SetFilter(neutronFilter);

  //TODO: Add scorers here if necessary. Follow recipe in G4 example B03ImportanceDetectorConstruction.
}
