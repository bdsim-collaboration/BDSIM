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
#ifndef BDSIMPORTANCEDETECTORCONSTRUCTION_H
#define BDSIMPORTANCEDETECTORCONSTRUCTION_H

#include "BDSExtent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSImportanceVolumeStore.hh"
#include "BDSImportanceDetectorConstruction.hh"

#include "globals.hh" // geant4 types / globals
#include "G4IStore.hh"
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserParallelWorld.hh"
#include "G4GeometryCell.hh"
#include "G4GeometryCellComp.hh"
#include "G4VPhysicalVolume.hh"

#include <list>
#include <string>
#include <vector>

class G4LogicalVolume;
class G4Region;
class G4VPhysicalVolume;

namespace GMAD {
  struct Element;
  template<typename T> class FastList;
  class Placement;
}

class BDSShowerModel;

#if G4VERSION_NUMBER > 1009
class BDSBOptrMultiParticleChangeCrossSection;
#endif

/**
 * @brief Class that constructs a parallel importance world
 *
 *
 * @author Will Shields
 */

class BDSImportanceDetectorConstruction: public G4VUserParallelWorld
{
public:
    BDSImportanceDetectorConstruction();
  virtual ~BDSImportanceDetectorConstruction();

  /// Overridden Geant4 method that must be implemented. Constructs the Geant4 geometry
  /// and returns the finished world physical volume.
  void Construct();

  /// Create biasing operations.
  void BuildPhysicsBias();

  /// Public access to the world extent.
  BDSExtent WorldExtent() const {return worldExtent;}

  G4VPhysicalVolume &GetWorldVolumeAddress() const;

  BDSGeometryExternal* geom;

  G4String GetCellName(G4int i);

  G4GeometryCell GetGeometryCell(G4int i);

  void Add(G4IStore* aIstore);

private:
  /// assignment and copy constructor not implemented nor used
  BDSImportanceDetectorConstruction& operator=(const BDSImportanceDetectorConstruction&);
  BDSImportanceDetectorConstruction(BDSImportanceDetectorConstruction&);

  /// Create and set parameters for various G4Regions
  void InitialiseRegions();

  /// Build the world volume using the extent of the BDSBeamline instance created
  /// in BuildBeamline()
  void BuildWorld();

#if G4VERSION_NUMBER > 1009
  /// Function that creates physics biasing cross section
  BDSBOptrMultiParticleChangeCrossSection* BuildCrossSectionBias(const std::list<std::string>& biasList,
								 G4String defaultBias,
								 G4String elementName);

  /// List of bias objects - for memory management
  std::vector<BDSBOptrMultiParticleChangeCrossSection*> biasObjects;
#endif

#ifdef BDSDEBUG
  bool debug = true;
#else
  bool debug = false;
#endif

  ///@{ Variable copied from global constants
  G4bool verbose;
  G4bool checkOverlaps;
  ///@}

  G4VPhysicalVolume* imWorldPV;

  BDSExtent worldExtent; ///< Record of the world extent.

  std::vector< G4LogicalVolume * > fLogicalVolumeVector;

  BDSImportanceVolumeStore fPVolumeStore;

  std::map<G4String, G4double> imVolumesAndValues;
};

#endif

