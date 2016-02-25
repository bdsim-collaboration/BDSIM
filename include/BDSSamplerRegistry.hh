#ifndef BDSSAMPLERREGISTRY_H
#define BDSSAMPLERREGISTRY_H

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include <vector>

class BDSSampler;

/**
 * @brief Associated information for the placement of a sampler.
 *
 * This singleton registry stores the transform and S position for a
 * given sampler. This may simply be a placement of the same sampler object
 * again. It does not own the samplers. Registration of a sampler returns
 * an unique integer that should be used as the copy number for that placement
 * so that BDSSamplerSD will find the correct information for the sampler registered
 * here.  Also, this integer is used for output purposes as a unique output identifier.
 * Note, if a sampler is placed in the real mass world inside a component it may not be 
 * known at construction time where this will lie in the world, therefore a default
 * identity transform and s position can be used signalling BDSSamplerSD to look up 
 * the transform dynamically. The BDSSampler* registered in each case isn't currently
 * used, so a nullptr can also be registered safely - in case BDSSamplerSD is attached
 * to an arbitrary logical volume not part of a BDSSampler instance.
 * 
 * @author Laurie Nevay
 */

class BDSSamplerRegistry
{
public:
  /// Accessor for registry.
  static BDSSamplerRegistry* Instance();

  ~BDSSamplerRegistry();

  /// Register a sampler. This register an instance of a sampler
  /// (note could be same sampler object again and again) with a
  /// specific placement transform. The registration returns an
  /// integeter that the sampler transform, name etc can be looked
  /// up in this registry by - ie by the output. The name does not
  /// need to be unique. This registry ensures the returned integer
  /// will be unique however. Default transform is unity - ie up
  /// to developer (outside registry) to test on this and determine
  /// transform externally as can't be known at construction time.
  /// The transform is the transform the sampler is placed with in
  /// the world volume, so the inverse is required to get global to
  /// local transformation. S is global s position with unphysical
  /// default of -1m.
  G4int RegisterSampler(G4String      name,
			BDSSampler*   sampler,
			G4Transform3D transform = G4Transform3D(),
			G4double      S = -1000);

  /// @{ Accessor
  inline G4String      GetName(G4int name) const;
  inline BDSSampler*   GetSampler(G4int sampler) const;
  inline G4Transform3D GetTransform(G4int index) const;
  inline G4Transform3D GetTransformInverse(G4int index) const;
  inline G4double      GetSPosition(G4int index) const;
  /// @}

  /// Access all names at once
  inline std::vector<G4String> GetNames() const;

  /// Get number of registered samplers
  inline G4int NumberOfExistingSamplers() const;
  inline size_t size() const;
  
private:
  /// Private constructor to enforce singleton pattern
  BDSSamplerRegistry();

  /// Singleton instance
  static BDSSamplerRegistry* instance;

  /// Counter for easy checking of out of bounds and incrementing.
  /// Also the index in the member vectors, so zero counting.
  G4int numberOfEntries;

  /// @{ Storage of sampler info where index is reference integer.
  std::vector<G4String>      names;
  std::vector<BDSSampler*>   samplers;
  std::vector<G4Transform3D> transforms;
  std::vector<G4Transform3D> transformInverses;
  std::vector<G4double>      sPosition;
  /// @}
};

inline G4String BDSSamplerRegistry::GetName(G4int index) const
{return names.at(index);}

inline BDSSampler* BDSSamplerRegistry::GetSampler(G4int index) const
{return samplers.at(index);}

inline G4Transform3D BDSSamplerRegistry::GetTransform(G4int index) const
{return transforms.at(index);}

inline G4Transform3D BDSSamplerRegistry::GetTransformInverse(G4int index) const
{return transformInverses.at(index);}

inline G4double BDSSamplerRegistry::GetSPosition(G4int index) const
{return sPosition.at(index);}

inline std::vector<G4String> BDSSamplerRegistry::GetNames() const
{return names;}

inline G4int BDSSamplerRegistry::NumberOfExistingSamplers() const
{return numberOfEntries;}

inline size_t BDSSamplerRegistry::size() const
{return names.size();}

#endif
