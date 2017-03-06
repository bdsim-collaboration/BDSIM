#ifndef BDSFIELDINFO_H
#define BDSFIELDINFO_H

#include "BDSFieldFormat.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"

#include <ostream>

class BDSCavityInfo;
class BDSMagnetStrength;

/**
 * @brief All info required to build complete field of any type.
 * 
 * With an instance of this class, the field factory should be able
 * to create the desired fully functional field with all required 
 * associated Geant4 objects.
 * 
 * Owns the magnetic strength instance & cavity info instance.
 *
 * @author Laurie Nevay
 */

class BDSFieldInfo
{
public:
  /// Default constructor for zero field effectively
  BDSFieldInfo();
  BDSFieldInfo(BDSFieldType             fieldTypeIn,
	       G4double                 brhoIn,
	       BDSIntegratorType        integratorTypeIn,
	       const BDSMagnetStrength* magnetStrengthIn           = nullptr,
	       G4bool                   provideGlobalTransformIn   = true,
	       G4Transform3D            transformIn                = G4Transform3D(),
	       const BDSCavityInfo*     cavityInfoIn               = nullptr,
	       G4String                 magneticFieldFilePathIn    = "",
	       BDSFieldFormat           magneticFieldFormatIn      = BDSFieldFormat::bdsim1d,
	       BDSInterpolatorType      magneticInterpolatorTypeIn = BDSInterpolatorType::nearest3d,
	       G4String                 electricFieldFilePathIn    = "",
	       BDSFieldFormat           electricFieldFormatIn      = BDSFieldFormat::bdsim1d,
	       BDSInterpolatorType      electricInterpolatorTypeIn = BDSInterpolatorType::nearest3d,
	       G4bool                   cacheTransformsIn          = false,
	       G4double                 eScalingIn                 = 1.0,
	       G4double                 bScalingIn                 = 1.0,
	       G4double                 timeOffsetIn               = 0);
  ~BDSFieldInfo();

  /// Copy constructor
  BDSFieldInfo(const BDSFieldInfo& other);

  /// @{ Accessor
  inline BDSFieldType        FieldType()                const {return fieldType;}
  inline G4double            BRho()                     const {return brho;}
  inline BDSIntegratorType   IntegratorType()           const {return integratorType;}
  inline G4Transform3D       Transform()                const {return transform;}
  inline const BDSMagnetStrength*  MagnetStrength()     const {return magnetStrength;}
  inline const BDSCavityInfo*      CavityInfo()         const {return cavityInfo;}
  inline G4bool              ProvideGlobal()            const {return provideGlobalTransform;}
  inline G4String            MagneticFile()             const {return magneticFieldFilePath;}
  inline BDSFieldFormat      MagneticFormat()           const {return magneticFieldFormat;}
  inline BDSInterpolatorType MagneticInterpolatorType() const {return magneticInterpolatorType;}
  inline G4String            ElectricFile()             const {return electricFieldFilePath;}
  inline BDSFieldFormat      ElectricFormat()           const {return electricFieldFormat;}
  inline BDSInterpolatorType ElectricInterpolatorType() const {return electricInterpolatorType;}
  inline G4bool              CacheTransforms()          const {return cacheTransforms;}
  inline G4double            EScaling()                 const {return eScaling;}
  inline G4double            BScaling()                 const {return bScaling;}
  inline G4double            TimeOffset()               const {return timeOffset;}
  /// @}

  /// Set Transform - could be done afterwards once instance of this class is passed around.
  inline void SetTransform(G4Transform3D transformIn) {transform = transformIn;}

  /// Translate - adds an additional translation to the transform member variable. May only
  /// be known at assembly time given parameterised geometry. Used by AWAKE Spectrometer only.
  void Translate(G4ThreeVector translationIn);

  /// Turn on or off transform caching.
  inline void CacheTransforms(G4bool cacheTransformsIn) {cacheTransforms = cacheTransformsIn;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSFieldInfo const &info);
  
private:
  
  BDSFieldType             fieldType;
  G4double                 brho;
  BDSIntegratorType        integratorType;
  const BDSMagnetStrength* magnetStrength;
  G4bool                   provideGlobalTransform;
  G4Transform3D            transform;  ///< Transform w.r.t. solid field will be attached to
  const BDSCavityInfo*     cavityInfo;
  G4String                 magneticFieldFilePath;
  BDSFieldFormat           magneticFieldFormat;
  BDSInterpolatorType      magneticInterpolatorType;
  G4String                 electricFieldFilePath;
  BDSFieldFormat           electricFieldFormat;
  BDSInterpolatorType      electricInterpolatorType;
  G4bool                   cacheTransforms;
  G4double                 eScaling;
  G4double                 bScaling;
  G4double                 timeOffset;
};

#endif
