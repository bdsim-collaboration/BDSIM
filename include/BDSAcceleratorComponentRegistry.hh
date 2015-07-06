#ifndef BDSACCELERATORCOMPONENTREGISTRY_H
#define BDSACCELERATORCOMPONENTREGISTRY_H

/**
 * @brief A registry of constructed BDSAcceleratorComponent instances that
 * can be searched.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 globals / types

#include <iterator>
#include <map>

class BDSAcceleratorComponentRegistry
{
public:
  /// Singleton accessor
  static BDSAcceleratorComponentRegistry* Instance();

  /// Destructor resets instance to 0 so that the class can be reinstantiated
  /// if necessary.
  ~BDSAcceleratorComponentRegistry();

  /// Register an instance of logical volume info with the address of the logical
  /// volume, which is used as the key to access it. Optional isReadOutVolume flag
  /// means volume info will be stored in a separate register that will be searched
  /// first to minimise search time. If not found there, the main register will be
  /// searched.
  void RegisterComponent(BDSAcceleratorComponent* component);

  /// Check whether an accelerator component is already registered.
  G4bool IsRegistered(BDSAcceleratorComponent* component);

  /// Check whether an accelerator component is already registered by name.
  G4bool IsRegistered(G4String componentName);

  /// Access an already constructed component - will exit if no such component found
  BDSAcceleratorComponent* GetComponent(G4String name);

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSAcceleratorComponentRegistry const &r);

private:
  /// Default constructor is private as singleton
  BDSAcceleratorComponentRegistry();
  
  /// The singleton instane
  static BDSAcceleratorComponentRegistry* _instance;

  /// Registry is a map - note 'register' is a protected keyword.
  std::map<G4String, BDSAcceleratorComponent*> registry;
};






#endif
