#ifndef BDSLOGICALVOLUMEINFOREGISTRY
#define BDSLOGICALVOLUMEINFOREGISTRY

/**
 * @brief A registry of all logical volume info instances that
 * can be searched.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"

#include <iterator>
#include <map>


class BDSLogicalVolumeInfo;

typedef std::map<G4LogicalVolume*, BDSLogicalVolumeInfo*>::iterator BDSLVInfoIterator;

class BDSLogicalVolumeInfoRegistry
{
public:
  /// Singleton accessor
  static BDSLogicalVolumeInfoRegistry* Instance();

  /// Destructor resets instance to 0 so that the class can be reinstantiated
  /// if necessary
  ~BDSLogicalVolumeInfoRegistry();

  /// Register an instance of logical volume info with the address of the logical
  /// volume, which is used as the key to access it
  void RegisterInfo(G4LogicalVolume* lvPointer, BDSLogicalVolumeInfo* info);

  /// Get the logical volume info for a particular logical volume (by address). Note,
  /// returns null pointer if none found
  BDSLogicalVolumeInfo* GetInfo(G4LogicalVolume* logicalVolume);

private:
  /// Default constructor is private as singleton
  BDSLogicalVolumeInfoRegistry();

  /// The singleton instane
  static BDSLogicalVolumeInfoRegistry* _instance;

  /// Registry is a map - note 'register' is a protected keyword. 
  std::map<G4LogicalVolume*, BDSLogicalVolumeInfo*> theRegister;

};






#endif
