#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H

#include "BDSGeometryCompoennt.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include <vector>

/**
 * @brief Base class and registry of sampler instances.
 * 
 * 
 * @author Laurie Nevay
 */

class BDSSampler: public BDSGeometryComponent
{
public:
  BDSSampler(G4String       nameIn,
	     G4Transform3D* transformIn);
  virtual ~BDSSampler();

  /// Return the number of existing samplers
  static inline G4int NumberOfExisingSamplers() const;

  /// Access the name of a registered (by construction) sampler.
  static inline G4String GetSamplerName(G4int index) const;

  /// Access the transform (local to global) that was used to place the sampler
  /// in the world volume.
  static inline G4Transform3D* GetSamplerTransform(G4int index) const;

  /// Access the inverse transform (ie global to local) of the one used to
  /// place the sampler in the world volume.
  static inline G4Transform3D* GetSamplerTransformInverse(G4int index) const;

protected:
  void CommonConstruction();

private:
  /// The name of this sampler
  G4String name;
  
  /// The ID number of this sampler instance. 0 counting.
  G4int samplerID;

  /// The total number of samplers that have been created. Starts at
  /// 0, so after first sampler is created this is 1.
  static G4int                       totalNumberOfSamplers;

  /// A register of the placement transform for each sampler in the
  /// world volume in order they were created - so that the sampler
  /// ID number is the index in the vector of that samplers information.
  static std::vector<G4Transform3D*> transforms;

  /// A register of calculated inverse transforms from the supplied placement
  /// transform the sampler was placed with.
  static std::vector<G4Transform3D*> inverseTransforms;
  
  /// A register of the object name for each sampler. In order as constructed.
  static std::vector<G4String>       names;
};

inline G4int BDSSampler::NumberOfExistingSamplers() const
{return totalNumberOfSamplers;}

inline G4String BDSSampler::GetSamplerName(G4int index) const
{return names[index];}

inline G4Transform3D* BDSSampler::GetTransform(G4int index) const
{return transforms[index];}

inline G4Transform3D* BDSSampler::GetTransformInverse(G4int index) const
{return inverseTransforms[index];}


#endif
