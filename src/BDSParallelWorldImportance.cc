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

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <vector>

BDSParallelWorldImportance::BDSParallelWorldImportance():
    G4VUserParallelWorld("importanceWorld")
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verbose       = globals->Verbose();
  checkOverlaps = globals->CheckOverlaps();
}

void BDSParallelWorldImportance::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(BDSGlobalConstants::Instance()->ImportanceVolumeMapFile());
  imVolumesAndValues = BDSImportanceFileLoader::Instance()->Load(importanceMapFile);

  // build world
  BuildWorld();
}

BDSParallelWorldImportance::~BDSParallelWorldImportance()
{;}

void BDSParallelWorldImportance::BuildWorld()
{
  // load the importance world geometry
  std::string geometryFile = BDSGlobalConstants::Instance()->ImportanceWorldGeometryFile();
  geom = BDSGeometryFactory::Instance()->BuildGeometry("importanceWorld", geometryFile, nullptr, 0, 0);

  //clone mass world for parallel world PV
  imWorldPV = GetWorld();

  G4LogicalVolume *worldLV = geom->GetContainerLogicalVolume();

  G4VisAttributes* samplerWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->VisibleDebugVisAttr()));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(samplerWorldVis);

  // set limits
  worldLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  fLogicalVolumeVector.push_back(worldLV);
  fPVolumeStore.AddPVolume(G4GeometryCell(*imWorldPV, 0));

  // add all logical volumes inside parallel world to the fLogicalVolumeVector vector
  // add all physical volumes inside parallel world to the fPVolumeStore

  //SetSensitive();
}

G4VPhysicalVolume &BDSParallelWorldImportance::GetWorldVolumeAddress() const{
  return *imWorldPV;
}

G4GeometryCell BDSParallelWorldImportance::GetGeometryCell(G4int i){

  std::ostringstream os;
  os << "cell_";
  if (i<10) {
    os << "0";
  }
  os << i;
  G4String PVname = os.str();

  const G4VPhysicalVolume *p=0;
  p = fPVolumeStore.GetPVolume(PVname);
  if (p)
    {return G4GeometryCell(*p,0);}
  else {
    G4cout << __METHOD_NAME__ << G4endl
           << " couldn't get G4GeometryCell" << G4endl;
    return G4GeometryCell(*imWorldPV,-2);
  }
}

G4String BDSParallelWorldImportance::GetCellName(G4int i) {
  std::ostringstream os;
  os << "cell_";
  if (i<10) {
    os << "0";
  }
  os << i;
  G4String name = os.str();
  return name;
}

void BDSParallelWorldImportance::Add(G4IStore* aIstore) {
  // set importance values and create scorers
  G4int numCells = (G4int) imVolumesAndValues.size();

  G4int cell(1);
  for (cell = 1; cell <= numCells; cell++) {
    G4GeometryCell gCell = GetGeometryCell(cell);
    G4double imp = std::pow(2.0, cell - 1);

    aIstore->AddImportanceGeometryCell(imp, gCell.GetPhysicalVolume(), cell);
  }

// create importance geometry cell pair for the "rest"cell
// with the same importance as the last concrete cell
  G4GeometryCell gCell = GetGeometryCell(numCells+1);

  G4double imp = std::pow(2.0, numCells-1);
  aIstore->AddImportanceGeometryCell(imp, gCell.GetPhysicalVolume(), numCells+1);
}