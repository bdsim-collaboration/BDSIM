#ifndef BDSFIELDINFO_H
#define BDSFIELDINFO_H

#include "BDSFieldFormat.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

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
  BDSFieldInfo(BDSFieldType       fieldTypeIn,
	       G4double           brhoIn,
	       BDSIntegratorType  integratorTypeIn,
	       BDSMagnetStrength* magnetStrengthIn         = nullptr,
	       G4bool             provideGlobalTransformIn = true,
	       G4Transform3D      transformIn              = G4Transform3D(),
	       BDSCavityInfo*     cavityInfoIn             = nullptr,
	       G4String           magneticFieldFilePathIn  = "",
	       BDSFieldFormat     magneticFieldFormatIn    = BDSFieldFormat::bdsim3d,
	       G4String           electricFieldFilePathIn  = "",
	       BDSFieldFormat     electricFieldFormatIn    = BDSFieldFormat::bdsim3d,
	       G4bool             cacheTransformsIn        = true);
  ~BDSFieldInfo();

  /// Copy constructor
  BDSFieldInfo(const BDSFieldInfo& other);

  /// @{ Accessor
  inline BDSFieldType       FieldType()      const {return fieldType;}
  inline G4double           BRho()           const {return brho;}
  inline BDSIntegratorType  IntegratorType() const {return integratorType;}
  inline G4Transform3D      Transform()      const {return transform;}
  inline BDSMagnetStrength* MagnetStrength() const {return magnetStrength;}
  inline BDSCavityInfo*     CavityInfo()     const {return cavityInfo;}
  inline G4bool             ProvideGlobal()  const {return provideGlobalTransform;}
  inline G4String           MagneticFile()   const {return magneticFieldFilePath;}
  inline BDSFieldFormat     MagneticFormat() const {return magneticFieldFormat;}
  inline G4String           ElectricFile()   const {return electricFieldFilePath;}
  inline BDSFieldFormat     ElectricFormat() const {return electricFieldFormat;}
  inline G4bool             CacheTransforms()const {return cacheTransforms;}
  /// @}

  /// Set Transform - could be done afterwards once instance of this class is passed around.
  inline void SetTransform(G4Transform3D transformIn) {transform = transformIn;}

  /// Turn on or off transform caching.
  inline void CacheTransforms(G4bool cacheTransformsIn) {cacheTransforms = cacheTransformsIn;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSFieldInfo const &info);
  
private:
  
  BDSFieldType       fieldType;
  G4double           brho;
  BDSIntegratorType  integratorType;
  BDSMagnetStrength* magnetStrength;
  G4bool             provideGlobalTransform;
  G4Transform3D      transform;  ///< Transform w.r.t. solid field will be attached to
  BDSCavityInfo*     cavityInfo;
  G4String           magneticFieldFilePath;
  BDSFieldFormat     magneticFieldFormat;
  G4String           electricFieldFilePath;
  BDSFieldFormat     electricFieldFormat;
  G4bool             cacheTransforms;
};

#endif
