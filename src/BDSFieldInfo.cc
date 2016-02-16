#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

BDSFieldInfo::BDSFieldInfo(BDSFieldType       fieldTypeIn,
			   G4double           brhoIn,
			   BDSIntegratorType  integratorTypeIn,
			   G4Transform3D      transformIn,
			   BDSMagnetStrength* magnetStrengthIn,
			   G4String           magneticFieldFilePathIn,
			   G4String           electricFieldFilePathIn):
  fieldType(fieldTypeIn),
  brho(brhoIn),
  integratorType(integratorTypeIn),
  transform(transformIn),
  magnetStrength(magnetStrengthIn),
  magneticFieldFilePath(magneticFieldFilePathIn),
  electricFieldFilePath(electricFieldFilePathIn)
{;}

BDSFieldInfo::~BDSFieldInfo()
{
  delete magnetStrength;
}
