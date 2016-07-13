#include "BDSCavityInfo.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include <ostream>

BDSFieldInfo::BDSFieldInfo():
        fieldType(BDSFieldType::none),
        brho(0),
        integratorType(BDSIntegratorType::none),
        magnetStrength(nullptr),
        provideGlobalTransform(false),
        transform(G4Transform3D()),
        cavityInfo(nullptr),
        magneticFieldFilePath(""),
        magneticFieldFormat(BDSFieldFormat::none),
        electricFieldFilePath(""),
        electricFieldFormat(BDSFieldFormat::none)
{;}

BDSFieldInfo::BDSFieldInfo(BDSFieldType       fieldTypeIn,
			   G4double           brhoIn,
			   BDSIntegratorType  integratorTypeIn,
			   BDSMagnetStrength* magnetStrengthIn,
			   G4bool             provideGlobalTransformIn,
			   G4Transform3D      transformIn,
			   BDSCavityInfo*     cavityInfoIn,
			   G4String           magneticFieldFilePathIn,
			   BDSFieldFormat     magneticFieldFormatIn,
			   G4String           electricFieldFilePathIn,
			   BDSFieldFormat     electricFieldFormatIn):
  fieldType(fieldTypeIn),
  brho(brhoIn),
  integratorType(integratorTypeIn),
  magnetStrength(magnetStrengthIn),
  provideGlobalTransform(provideGlobalTransformIn),
  transform(transformIn),
  cavityInfo(cavityInfoIn),
  magneticFieldFilePath(magneticFieldFilePathIn),
  magneticFieldFormat(magneticFieldFormatIn),
  electricFieldFilePath(electricFieldFilePathIn),
  electricFieldFormat(electricFieldFormatIn)
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
  magneticFieldFormat(other.magneticFieldFormat),
  electricFieldFilePath(other.electricFieldFilePath),
  electricFieldFormat(other.electricFieldFormat)
{
  magnetStrength = new BDSMagnetStrength(*other.magnetStrength);
  cavityInfo     = new BDSCavityInfo(*other.cavityInfo);
}

std::ostream& operator<< (std::ostream& out, BDSFieldInfo const& info)
{
  out << "Field type:        " << info.fieldType              << G4endl;
  out << "Rigidity:          " << info.brho                   << G4endl;
  out << "Integrator:        " << info.integratorType         << G4endl;
  out << "Magnet strength:   " << *(info.magnetStrength)      << G4endl;
  out << "Global transform?  " << info.provideGlobalTransform << G4endl;
  out << "B map file:        " << info.magneticFieldFilePath  << G4endl;
  out << "B map file format: " << info.magneticFieldFormat    << G4endl;
  out << "E map file:        " << info.electricFieldFilePath  << G4endl;
  out << "E map file format: " << info.electricFieldFormat    << G4endl;
  return out;
}
