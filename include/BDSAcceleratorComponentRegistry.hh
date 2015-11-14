#ifndef BDSACCELERATORCOMPONENTREGISTRY_H
#define BDSACCELERATORCOMPONENTREGISTRY_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 globals / types

#include <iterator>
#include <map>

/**
 * @brief A registry of constructed BDSAcceleratorComponent instances that
 * can be searched. 
 * 
 * Uses an std::map rather than unordered map as although 
 * slower to access (less often and only at construction), a map is faster 
 * for iterating than an unordered map, which will be required to apply 
 * wrapper physics processes - relatively common.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSAcceleratorComponentRegistry
{
private:
  /// Typedefs up first so we can declare public iterators.
  /// Use a typedef for this specific map implementation so we can easily
  /// define iterators and internal member variables without risking getting
  /// the exact map declaration wrong. 
  typedef std::map<G4String, BDSAcceleratorComponent*> RegistryMap;

  /// Registry is a map - note 'register' is a protected keyword.
  RegistryMap registry;
  
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

  /// Access an already constructed component - will return null if no such component found.
  /// This is safe as this registry is primarily used by BDSComponentFactory which can return
  /// NULL to BDSDetectorConstruction safely if an invalid component is requested.
  BDSAcceleratorComponent* GetComponent(G4String name);

  typedef RegistryMap::iterator       iterator;
  typedef RegistryMap::const_iterator const_iterator;
  iterator       begin()       {return registry.begin();}
  iterator       end()         {return registry.end();}
  const_iterator begin() const {return registry.begin();}
  const_iterator end()   const {return registry.end();}
  G4bool         empty() const {return registry.empty();}
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSAcceleratorComponentRegistry const &r);

private:
  /// Default constructor is private as singleton
  BDSAcceleratorComponentRegistry();
  
  /// The singleton instane
  static BDSAcceleratorComponentRegistry* _instance;

  /// assignment and copy constructor not implemented nor used
  BDSAcceleratorComponentRegistry& operator=(const BDSAcceleratorComponentRegistry&);
  BDSAcceleratorComponentRegistry(BDSAcceleratorComponentRegistry&);
};






#endif
