/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "BDSBeamlineSet.hh"
#include "BDSScorerHistogramDef.hh"

#include "globals.hh"         // geant4 globals / types

#include <map>
#include <set>
#include <vector>

class BDSAperture;
class BDSBeamline;
class BDSFieldObjects;
class BDSRegion;
class G4LogicalVolume;
class G4Region;
class G4VPhysicalVolume;
class G4VSolid;
class G4VUserParallelWorld;

/**
 * @brief A holder class for all representations of the
 * accelerator model created in BDSIM. 
 *
 * This can be extended to 
 * allow inspection of the model. Holds the readout geometry
 * physical world in a location independent of detector 
 * construction.
 * 
 * In future, there may be several more beamlines than just
 * a flat one, and perhaps grouped into a more hierarchical
 * version.  These can be contained here and this class can
 * be extended as required.
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorModel
{
public:
  static BDSAcceleratorModel* Instance();
  ~BDSAcceleratorModel();

  /// @{ Register consituent of world.
  inline void RegisterWorldPV(G4VPhysicalVolume* worldIn) {worldPV = worldIn;}
  inline void RegisterWorldLV(G4LogicalVolume*   worldIn) {worldLV = worldIn;}
  inline void RegisterWorldSolid(G4VSolid*       worldIn) {worldSolid = worldIn;}
  /// @}

  inline G4VPhysicalVolume* WorldPV() const {return worldPV;} ///< Access the physical volume of the world.
  inline G4LogicalVolume*   WorldLV() const {return worldLV;} ///< Access the logical volume of the world.

  /// Register the main beam line set.
  void RegisterBeamlineSetMain(const BDSBeamlineSet& setIn);

  /// Register a set of beam lines to be managed and cleared up at the end of the simulation.
  void RegisterBeamlineSetExtra(const G4String&       name,
				                const BDSBeamlineSet& setIn);
  
  /// @{ Accessor.
  inline const BDSBeamlineSet& BeamlineSetMain() const {return mainBeamlineSet;}
  const BDSBeamlineSet& BeamlineSet(const G4String& name) const;
  inline const std::map<G4String, BDSBeamlineSet>& ExtraBeamlines() const {return extraBeamlines;}
  const BDSBeamline* BeamlineMain() const {return mainBeamlineSet.massWorld;}
  /// @}

  inline void RegisterParallelWorld(G4VUserParallelWorld* world) {parallelWorlds.insert(world);}

  /// Register the  beam line of arbitrary placements.
  inline void RegisterPlacementBeamline(BDSBeamline* placementBLIn) {placementBeamline = placementBLIn;}

  /// Access the beam line of arbitrary placements.
  inline BDSBeamline* PlacementBeamline() const {return placementBeamline;}

  /// Register a beam line of blm placements.
  inline void RegisterBLMs(BDSBeamline* blmsIn) {blmsBeamline = blmsIn;}

  /// Access the beam line of blm placements.
  inline BDSBeamline* BLMsBeamline() const {return blmsBeamline;}
  
  /// Register the beam line containing all the tunnel segments
  inline void RegisterTunnelBeamline(BDSBeamline* beamlineIn) {tunnelBeamline = beamlineIn;}

  /// Access the beam line containing all the tunnel segments
  inline BDSBeamline* TunnelBeamline() const {return tunnelBeamline;}
  
  /// Register all field objects
  inline void RegisterFields(std::vector<BDSFieldObjects*>& fieldsIn){fields = fieldsIn;}

  /// Register a region.
  void RegisterRegion(BDSRegion* region);

  /// Register a single aperture.
  inline void RegisterAperture(G4String name, BDSAperture* apertureIn) {apertures[name] = apertureIn;}
  
  /// Register a map of apertures with associated names.
  void RegisterApertures(const std::map<G4String, BDSAperture*>& aperturesIn);

  /// Access an aperture definition. Will throw an exception if not found. Note,
  /// we use pointers as we purposively don't provide a default constructor for BDSAperture
  /// as required by an std::map.
  BDSAperture*  Aperture(G4String name) const;

  /// Access region information. Will exit if not found.
  G4Region* Region(G4String name) const;

  /// Returns pointer to a set of logical volumes. If no set by that name exits, create it.
  std::set<G4LogicalVolume*>* VolumeSet(G4String name);

  /// Check whether a volume is in a registry of volumes (a set). If no such registry exists
  /// then return false.
  G4bool VolumeInSet(G4LogicalVolume* volume,
		     G4String registryName);

  /// Find a corresponding mass world beam line for a curvilinear (or bridge) beam
  /// line from the registered beam line sets.
  BDSBeamline* CorrespondingMassWorldBeamline(BDSBeamline* bl) const;

  /// Return whether a beam line is a mass world beam line. If in the unlikely event the
  /// beam line isn't registered, false is returned by default.
  G4bool BeamlineIsMassWorld(BDSBeamline* bl) const;

  /// Update a beam line pointer and index if required for the equivalent ones in the mass
  /// world beam line. If the beam line supplied is a mass world one, nothing is done.
  void MassWorldBeamlineAndIndex(BDSBeamline*& bl,
				 G4int&        index) const;

  /// Register a scorer histogram definition so it can be used in the output. The definition
  /// is stored both in a vector and a map. Note, repeated entries will exist in the vector
  /// but be overwritten in the map.
  void RegisterScorerHistogramDefinition(const BDSScorerHistogramDef& def);

  /// @{ Access all scorer histogram definitions.
  const std::vector<BDSScorerHistogramDef>& ScorerHistogramDefinitions() const {return scorerHistogramDefs;}
  const std::map<G4String, BDSScorerHistogramDef>& ScorerHistogramDefinitionsMap() const {return scorerHistogramDefsMap;}
  const BDSScorerHistogramDef* ScorerHistogramDef(const G4String& name);
  /// @}
  
private:
  BDSAcceleratorModel(); ///< Default constructor is private as singleton.

  static BDSAcceleratorModel* instance;

  G4VPhysicalVolume* worldPV;              ///< Physical volume of the mass world.
  G4LogicalVolume*   worldLV;
  G4VSolid*          worldSolid;

  BDSBeamlineSet mainBeamlineSet;
  std::map<G4String, BDSBeamlineSet> extraBeamlines; ///< Extra beamlines.

  std::set<G4VUserParallelWorld*> parallelWorlds; ///< Parallel worlds not use with beam lines

  /// Mapping from any curvilinear beam line to the corresponding mass world beam line.
  std::map<BDSBeamline*, BDSBeamline*> clToMassWorldMap;

  /// Map from beam line pointer to whether that beam line object is a mass world one,
  /// i.e. not a curvilinear or bridge one. 'TF' for true false.
  std::map<BDSBeamline*, G4bool> massWorldMapTF;

  /// Utility function to apply mapping.
  void MapBeamlineSet(const BDSBeamlineSet& setIn);

  BDSBeamline* tunnelBeamline;            ///< Tunnel segments beam line.
  BDSBeamline* placementBeamline;         ///< Placement beam line.
  BDSBeamline* blmsBeamline;              ///< BLMs beam line.
  
  std::vector<BDSFieldObjects*>         fields;    ///< All field objects.
  std::map<G4String, BDSRegion*>        regions;
  std::set<BDSRegion*>                  regionStorage; ///< Unique storage of regions.
  std::map<G4String, BDSAperture*>  apertures; ///< All apertures.

  std::map<G4String, std::set<G4LogicalVolume*>* > volumeRegistries; ///< All volume registries.

  /// @{ Scorer histogram definitions cached from construction here to be used in output creation.
  std::vector<BDSScorerHistogramDef> scorerHistogramDefs;
  std::map<G4String, BDSScorerHistogramDef> scorerHistogramDefsMap;
  /// @}
};

#endif
