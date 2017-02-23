#ifndef BDSFIELDBUILDER_H
#define BDSFIELDBUILDER_H

#include "globals.hh" //G4 global constants & types
#include <vector>

class BDSFieldInfo;
class BDSFieldObjects;
class G4LogicalVolume;

/**
 * @brief Register for all fields to be built and volumes to be attached to.
 * 
 * This keeps a registry of the field info (the template for a field) and the 
 * volume to attach it to, so all fields can actually be constructed at one
 * time irrespective of when they're apparently 'created' in the geometry / 
 * individual BDSAcceleratorComponents.
 * 
 * This register does not retain ownership of anything so multiple logical volumes
 * may be safely registered along with a single BDSFieldInfo instance.
 *
 * @author Laurie Nevay
 */

class BDSFieldBuilder
{
public:
  /// Singleton pattern accessor.
  static BDSFieldBuilder* Instance();

  ~BDSFieldBuilder();

  /// Register a field for construction - ie the field specification along with
  /// which logical volume to attach it to. The same field specification (info)
  /// can be registered for multiple logical volumes as this builder does not
  /// retain ownership of anything.
  void RegisterFieldForConstruction(const BDSFieldInfo* info,
				    G4LogicalVolume*    logicalVolume,
				    const G4bool        propagateToDaughters = false);

  /// Similar version but 
  void RegisterFieldForConstruction(const BDSFieldInfo* info,
				    const std::vector<G4LogicalVolume*>& logicalVolumes,
				    const G4bool              propagateToDaughters = false);

  std::vector<BDSFieldObjects*> CreateAndAttachAll();

private:
  /// Private default constructor to enforce singleton pattern.
  BDSFieldBuilder();

  /// Singleton instance.
  static BDSFieldBuilder* instance;
  
  /// @{ Register of components to build.
  std::vector<const BDSFieldInfo*> infos;
  std::vector<std::vector<G4LogicalVolume*> > lvs;
  std::vector<G4bool>              propagators;
  /// @}
};

#endif
