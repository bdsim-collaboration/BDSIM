#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H

#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include <vector>

/**
 * @brief Base class and registry of sampler instances.
 * 
 * Retruns G4Transform3D identity in case none is set, which can be tested
 * against G4Transfrom3D::Identity.
 * 
 * @author Laurie Nevay
 */

class BDSSampler: public BDSGeometryComponent
{
public:
  BDSSampler(G4String      name,
	     G4Transform3D transform);
  virtual ~BDSSampler(){;}

  /// Return the ID number of this sampler once created
  inline G4int GetID() const;

  /// Return the name of this sampler.
  inline G4String GetName() const;

  /// Return the number of existing samplers
  static inline G4int NumberOfExistingSamplers();

  /// Access the name of a registered sampler by index.
  static inline G4String GetName(G4int index);

  /// Access all the names at once - useful for output building.
  static inline std::vector<G4String> GetNames();

  /// Access the transform (local to global) that was used to place the sampler
  /// in the world volume.
  static inline G4Transform3D GetTransform(G4int index);

  /// Access the inverse transform (ie global to local) of the one used to
  /// place the sampler in the world volume.
  static inline G4Transform3D GetTransformInverse(G4int index);

  /// Register an externally constructed sampler (from another class). If transform
  /// isn't known at time of construction (as likely inside an accelerator component),
  /// the sampler SD will look it up from the volume rather than using the cache.
  static G4int AddExternalSampler(G4String      name,
				  G4Transform3D transform = G4Transform3D());
  
protected:
  /// Common construction tasks such as creating a logical volume from the solid
  /// and visualisation options.
  void CommonConstruction();

private:  
  /// The ID number of this sampler instance. 0 counting.
  G4int samplerID;

  /// The total number of samplers that have been created. Starts at
  /// 0, so after first sampler is created this is 1.
  static G4int                       totalNumberOfSamplers;

  /// A register of the placement transform for each sampler in the
  /// world volume in order they were created - so that the sampler
  /// ID number is the index in the vector of that samplers information.
  static std::vector<G4Transform3D> transforms;

  /// A register of calculated inverse transforms from the supplied placement
  /// transform the sampler was placed with.
  static std::vector<G4Transform3D> inverseTransforms;
  
  /// A register of the object name for each sampler. In order as constructed.
  static std::vector<G4String> names;
};

inline G4int BDSSampler::GetID() const
{return samplerID;}

inline G4String BDSSampler::GetName() const
{return names[samplerID];}

inline std::vector<G4String> BDSSampler::GetNames()
{return names;}

inline G4int BDSSampler::NumberOfExistingSamplers()
{return totalNumberOfSamplers;}

inline G4String BDSSampler::GetName(G4int index)
{return names[index];}

inline G4Transform3D BDSSampler::GetTransform(G4int index)
{return transforms[index];}

inline G4Transform3D BDSSampler::GetTransformInverse(G4int index)
{return inverseTransforms[index];}


#endif
