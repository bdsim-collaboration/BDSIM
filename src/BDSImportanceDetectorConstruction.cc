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
#include "BDSImportanceDetectorConstruction.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSImportanceFileLoader.hh"
#include "BDSMaterials.hh"
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

BDSImportanceDetectorConstruction::BDSImportanceDetectorConstruction():
    G4VUserParallelWorld("importanceWorld")
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verbose       = globals->Verbose();
  checkOverlaps = globals->CheckOverlaps();
}

void BDSImportanceDetectorConstruction::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(BDSGlobalConstants::Instance()->ImportanceVolumeMap());
  imVolumesAndValues = BDSImportanceFileLoader::Instance()->Load(importanceMapFile);

  // build world
  BuildWorld();

  if(verbose || debug) G4cout << __METHOD_NAME__ << "detector Construction done"<<G4endl;
}

BDSImportanceDetectorConstruction::~BDSImportanceDetectorConstruction()
{;}

void BDSImportanceDetectorConstruction::BuildWorld()
{
  // load the importance world geometry
  std::string geometryFile = BDSGlobalConstants::Instance()->ImportanceWorldGeometryFile();
  geom = BDSGeometryFactory::Instance()->BuildGeometry("importanceWorld", geometryFile, nullptr, 0, 0);

  G4String worldName = "ImportanceWorld";
  G4LogicalVolume *worldLV = geom->GetContainerLogicalVolume();

  // visual attributes
  // copy the debug vis attributes but change to force wireframe
  G4VisAttributes *debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->ContainerVisAttr()));
  debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(debugWorldVis);

  // set limits
  worldLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  // place the world
  imWorldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
						 (G4ThreeVector)0,     // at (0,0,0)
						 worldLV,	           // its logical volume
						 worldName,            // its name
						 nullptr,		       // its mother  volume
						 false,		           // no boolean operation
						 0,                    // copy number
						 checkOverlaps);       // overlap checking

  fLogicalVolumeVector.push_back(worldLV);
  fPVolumeStore.AddPVolume(G4GeometryCell(*imWorldPV, 0));

  // add all logical volumes inside parallel world to the fLogicalVolumeVector vector
  // add all physical volumes inside parallel world to the fPVolumeStore

  //SetSensitive();
}

G4VPhysicalVolume &BDSImportanceDetectorConstruction::GetWorldVolumeAddress() const{
  return *imWorldPV;
}

G4GeometryCell BDSImportanceDetectorConstruction::GetGeometryCell(G4int i){

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
    G4cout << "BDSImportanceDetectorConstruction::GetGeometryCell: " << G4endl
           << " couldn't get G4GeometryCell" << G4endl;
    return G4GeometryCell(*imWorldPV,-2);
  }
}

G4String BDSImportanceDetectorConstruction::GetCellName(G4int i) {
  std::ostringstream os;
  os << "cell_";
  if (i<10) {
    os << "0";
  }
  os << i;
  G4String name = os.str();
  return name;
}

void BDSImportanceDetectorConstruction::Add(G4IStore* aIstore) {
  // set importance values and create scorers
  G4int numCells = imVolumesAndValues.size();

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