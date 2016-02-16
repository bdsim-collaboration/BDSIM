#ifndef BDSFIELDINFO_H
#define BDSFIELDINFO_H

#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

class BDSMagnetStrength;

/**
 * @brief All info required to build complete field of any type.
 * 
 * With an instance of this class, the field factory should be able
 * to create the desired fully functional field with all required 
 * associated Geant4 objects.
 * 
 * Owns the magnetic strength instance.
 *
 * @author Laurie Nevay
 */

class BDSFieldInfo
{
public:
  BDSFieldInfo(BDSFieldType       fieldTypeIn,
	       G4double           brhoIn,
	       BDSIntegratorType  integratorTypeIn,
	       G4Transform3D      transformIn             = G4Transform3D(),
	       BDSMagnetStrength* magnetStrengthIn        = nullptr,
	       G4String           magneticFieldFilePathIn = "",
	       G4String           electricFieldFilePathIn = "");
  ~BDSFieldInfo();

  const BDSFieldType       fieldtype;
  const G4double           brho;
  const BDSIntegratorType  integratorType;
  const G4Transform3D      transform;  ///< Transform w.r.t. solid field will be attached to
  BDSMagnetStrength const* magnetStrength;
  const G4String           magneticFieldFilePath;
  const G4String           electricFieldFilePath;

private:
  /// Private default constructor to force use of provided one.
  BDSFieldInfo();

};

#endif
