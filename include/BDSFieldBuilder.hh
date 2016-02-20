#ifndef BDSFIELDBUILDER_H
#define BDSFIELDBUILDER_H

#include "BDSFieldInfo.hh"
#include "BDSFieldObjects.hh"

#include "G4LogicalVolume.hh"

#include <vector>

/**
 * @brief Register for all fields to be built and volumes to be attached to.
 * 
 * This keeps a registry of the field info (the template for a field) and the 
 * volume to attach it to, so all fields can actually be constructed at one
 * time irrespective of when they're apparently 'created' in the geometry / 
 * individual BDSAcceleratorComponents.
 *
 * @author Laurie Nevay
 */

class BDSFieldBuilder
{
public:
  /// Singleton pattern accessor.
  static BDSFieldBuilder* Instance();

  ~BDSFieldBuilder();

  void RegisterFieldForConstruction(BDSFieldInfo* info,
				    G4LogicalVolume* logicalVolume,
				    G4bool           propagateToDaughters = false);

  std::vector<BDSFieldObjects*> CreateAndAttachAll();

private:
  /// Private default constructor to enforce singleton pattern.
  BDSFieldBuilder();

  /// Singleton instance.
  static BDSFieldBuilder* instance;
  
  /// @{ Register of components to build.
  std::vector<BDSFieldInfo*>    infos;
  std::vector<G4LogicalVolume*> lvs;
  std::vector<G4bool>           propagators;
  /// @}
};

#endif
