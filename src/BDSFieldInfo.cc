#include "BDSCavityInfo.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

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
	magneticInterpolatorType(BDSInterpolatorType::nearest3d),
        electricFieldFilePath(""),
        electricFieldFormat(BDSFieldFormat::none),
	electricInterpolatorType(BDSInterpolatorType::nearest3d),
	cacheTransforms(true),
	eScaling(1.0),
	bScaling(1.0),
	timeOffset(0)
{;}

BDSFieldInfo::BDSFieldInfo(BDSFieldType             fieldTypeIn,
			   G4double                 brhoIn,
			   BDSIntegratorType        integratorTypeIn,
			   const BDSMagnetStrength* magnetStrengthIn,
			   G4bool                   provideGlobalTransformIn,
			   G4Transform3D            transformIn,
			   const BDSCavityInfo*     cavityInfoIn,
			   G4String                 magneticFieldFilePathIn,
			   BDSFieldFormat           magneticFieldFormatIn,
			   BDSInterpolatorType      magneticInterpolatorTypeIn,
			   G4String                 electricFieldFilePathIn,
			   BDSFieldFormat           electricFieldFormatIn,
			   BDSInterpolatorType      electricInterpolatorTypeIn,
			   G4bool                   cacheTransformsIn,
			   G4double                 eScalingIn,
			   G4double                 bScalingIn,
			   G4double                 timeOffsetIn):
  fieldType(fieldTypeIn),
  brho(brhoIn),
  integratorType(integratorTypeIn),
  magnetStrength(magnetStrengthIn),
  provideGlobalTransform(provideGlobalTransformIn),
  transform(transformIn),
  cavityInfo(cavityInfoIn),
  magneticFieldFilePath(magneticFieldFilePathIn),
  magneticFieldFormat(magneticFieldFormatIn),
  magneticInterpolatorType(magneticInterpolatorTypeIn),
  electricFieldFilePath(electricFieldFilePathIn),
  electricFieldFormat(electricFieldFormatIn),
  electricInterpolatorType(electricInterpolatorTypeIn),
  cacheTransforms(cacheTransformsIn),
  eScaling(eScalingIn),
  bScaling(bScalingIn),
  timeOffset(timeOffsetIn)
{;}

BDSFieldInfo::~BDSFieldInfo()
{
  delete magnetStrength;
  delete cavityInfo;
}

BDSFieldInfo::BDSFieldInfo(const BDSFieldInfo& other):
  fieldType(other.fieldType),
  brho(other.brho),
  integratorType(other.integratorType),
  provideGlobalTransform(other.provideGlobalTransform),
  transform(other.transform),
  magneticFieldFilePath(other.magneticFieldFilePath),
  magneticFieldFormat(other.magneticFieldFormat),
  magneticInterpolatorType(other.magneticInterpolatorType),
  electricFieldFilePath(other.electricFieldFilePath),
  electricFieldFormat(other.electricFieldFormat),
  electricInterpolatorType(other.electricInterpolatorType),
  cacheTransforms(other.cacheTransforms),
  eScaling(other.eScaling),
  bScaling(other.bScaling),
  timeOffset(other.timeOffset)
{
  if (other.magnetStrength)
    {magnetStrength = new BDSMagnetStrength(*other.magnetStrength);}
  else
    {magnetStrength = other.magnetStrength;} // also nullptr
  cavityInfo     = new BDSCavityInfo(*other.cavityInfo);
}

std::ostream& operator<< (std::ostream& out, BDSFieldInfo const& info)
{
  out << "Field type:        " << info.fieldType                << G4endl;
  out << "Rigidity:          " << info.brho                     << G4endl;
  out << "Integrator:        " << info.integratorType           << G4endl;
  out << "Global transform?  " << info.provideGlobalTransform   << G4endl;
  out << "B map file:        " << info.magneticFieldFilePath    << G4endl;
  out << "B map file format: " << info.magneticFieldFormat      << G4endl;
  out << "B interpolator     " << info.magneticInterpolatorType << G4endl;
  out << "E map file:        " << info.electricFieldFilePath    << G4endl;
  out << "E map file format: " << info.electricFieldFormat      << G4endl;
  out << "E interpolator     " << info.electricInterpolatorType << G4endl;
  out << "Transform caching: " << info.cacheTransforms          << G4endl;
  out << "E Scaling:         " << info.eScaling                 << G4endl;
  out << "B Scaling:         " << info.bScaling                 << G4endl;
  out << "t offset           " << info.timeOffset               << G4endl;
  if (info.magnetStrength)
    {out << "Magnet strength:   " << *(info.magnetStrength)      << G4endl;}
  return out;
}

void BDSFieldInfo::Translate(G4ThreeVector translationIn)
{
  G4RotationMatrix       rm = transform.getRotation();
  G4ThreeVector translation = transform.getTranslation();
  translation += translationIn;
  transform = G4Transform3D(rm, translation);
}
