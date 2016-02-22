#include "BDSCavityInfo.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include <ostream>

BDSFieldInfo::BDSFieldInfo(BDSFieldType       fieldTypeIn,
			   G4double           brhoIn,
			   BDSIntegratorType  integratorTypeIn,
			   BDSMagnetStrength* magnetStrengthIn,
			   G4bool             provideGlobalTransformIn,
			   G4Transform3D      transformIn,
			   BDSCavityInfo*     cavityInfoIn,
			   G4String           magneticFieldFilePathIn,
			   G4String           electricFieldFilePathIn):
  fieldType(fieldTypeIn),
  brho(brhoIn),
  integratorType(integratorTypeIn),
  magnetStrength(magnetStrengthIn),
  provideGlobalTransform(provideGlobalTransformIn),
  transform(transformIn),
  cavityInfo(cavityInfoIn),
  magneticFieldFilePath(magneticFieldFilePathIn),
  electricFieldFilePath(electricFieldFilePathIn)
{;}

BDSFieldInfo::~BDSFieldInfo()
{
  delete magnetStrength;
}

BDSFieldInfo::BDSFieldInfo(const BDSFieldInfo& other):
  fieldType(other.fieldType),
  brho(other.brho),
  integratorType(other.integratorType),
  provideGlobalTransform(other.provideGlobalTransform),
  transform(other.transform),
  magneticFieldFilePath(other.magneticFieldFilePath),
  electricFieldFilePath(other.electricFieldFilePath)
{
  magnetStrength = new BDSMagnetStrength(*other.magnetStrength);
  cavityInfo     = new BDSCavityInfo(*other.cavityInfo);
}

std::ostream& operator<< (std::ostream& out, BDSFieldInfo const& info)
{
  out << "Field type:       " << info.fieldType              << G4endl;
  out << "Rigidity:         " << info.brho                   << G4endl;
  out << "Integrator:       " << info.integratorType         << G4endl;
  out << "Magnet strength:  " << *(info.magnetStrength)      << G4endl;
  out << "Global transform? " << info.provideGlobalTransform << G4endl;
  return out;
}
