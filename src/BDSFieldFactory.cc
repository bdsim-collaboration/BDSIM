#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldE.hh"
#include "BDSFieldEGlobal.hh"
#include "BDSFieldESinusoid.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobal.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMagDecapole.hh"
#include "BDSFieldMagDipoleQuadrupole.hh"
#include "BDSFieldMagGlobal.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSFieldMagOctupole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagSBend.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSFieldMagSkew.hh"
#include "BDSFieldMagSolenoid.hh"
#include "BDSFieldMagSQL.hh"
#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"
#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorDecapole.hh"
#include "BDSIntegratorDipole.hh"
#include "BDSIntegratorOctupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSIntegratorFringefield.hh"
#include "BDSIntegratorMultipole.hh"
#include "BDSIntegratorSextupole.hh"
#include "BDSIntegratorSolenoid.hh"
#include "BDSIntegratorTeleporter.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetType.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"

#include "parser/field.h"

#include "globals.hh" // geant4 types / globals
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4UniformMagField.hh"
// geant4 integrators
#include "G4CashKarpRKF45.hh"
#include "G4ClassicalRK4.hh"
#include "G4ConstRK4.hh"
#include "G4ExactHelixStepper.hh"
#include "G4ExplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixHeum.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixMixedStepper.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4ImplicitEuler.hh"
#include "G4NystromRK4.hh"
#include "G4RKG3_Stepper.hh"
#include "G4SimpleHeum.hh"
#include "G4SimpleRunge.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <map>
#include <typeinfo>
#include <utility>
#include <vector>

BDSFieldFactory* BDSFieldFactory::instance = nullptr;

BDSFieldFactory* BDSFieldFactory::Instance()
{
  if (instance == nullptr)
    {instance = new BDSFieldFactory();}
  return instance;
}

BDSFieldFactory::BDSFieldFactory()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  offset            = G4ThreeVector(0,0,0);
  format            = BDSFieldType::zero;
  fileName          = "";
  geometry          = nullptr;
  cacheLength       = 1*CLHEP::um;

  PrepareFieldDefinitions(BDSParser::Instance()->GetFields(),
			  BDSGlobalConstants::Instance()->BRho());
}

BDSFieldFactory::~BDSFieldFactory()
{
  for (auto info : parserDefinitions)
    {delete info.second;}
}

void BDSFieldFactory::PrepareFieldDefinitions(const std::vector<GMAD::Field>& definitions,
					      const G4double defaultBRho)
{
  for (const auto& definition : definitions)
    {
      BDSFieldType    fieldType = BDS::DetermineFieldType(definition.type);
      BDSIntegratorType intType = BDS::DetermineIntegratorType(definition.integrator);

      G4ThreeVector offset = G4ThreeVector(definition.x*CLHEP::m,
					   definition.y*CLHEP::m,
					   definition.z*CLHEP::m);

      G4RotationMatrix* rm = nullptr;
      if (definition.axisAngle)
	{
	  G4ThreeVector axis = G4ThreeVector(definition.axisX,
					     definition.axisY,
					     definition.axisZ);
	  rm = new G4RotationMatrix(axis, definition.angle*CLHEP::rad);
	}
      else
	{
	  if (BDS::IsFinite(definition.phi)   ||
	      BDS::IsFinite(definition.theta) ||
	      BDS::IsFinite(definition.psi))
	    {// only build if finite
	      CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(definition.phi*CLHEP::rad,
								definition.theta*CLHEP::rad,
								definition.psi*CLHEP::rad);
	      rm = new G4RotationMatrix(ang);
	    }
	  else
	    {rm = new G4RotationMatrix();}
	}
      
      G4Transform3D transform = G4Transform3D(*rm, offset);

      BDSFieldFormat magFormat = BDSFieldFormat::none;
      G4String       magFile   = "";
      G4bool  magFileSpecified = !definition.magneticFile.empty();
      if (magFileSpecified)
	{
	  std::pair<G4String, G4String> bf = BDS::SplitOnColon(G4String(definition.magneticFile));
	  magFormat = BDS::DetermineFieldFormat(bf.first);
	  magFile   = bf.second;
	}
      
      BDSFieldFormat eleFormat = BDSFieldFormat::none;
      G4String       eleFile   = "";
      G4bool  eleFileSpecified = !definition.electricFile.empty();
      if (eleFileSpecified)
	{
	  std::pair<G4String, G4String> ef = BDS::SplitOnColon(G4String(definition.electricFile));
	  eleFormat = BDS::DetermineFieldFormat(ef.first);
	  eleFile   = ef.second;
	}
      
      BDSInterpolatorType magIntType = BDSInterpolatorType::nearest3d;
      if (magFileSpecified) // will warn if no interpolator specified (default "")
	{magIntType = BDS::DetermineInterpolatorType(G4String(definition.magneticInterpolator));}
      BDSInterpolatorType eleIntType = BDSInterpolatorType::nearest2d;
      if (eleFileSpecified)
	{eleIntType = BDS::DetermineInterpolatorType(G4String(definition.electricInterpolator));}
      
      BDSFieldInfo* info = new BDSFieldInfo(fieldType,
					    defaultBRho,
					    intType,
					    nullptr, /*for now, no parameterised strenghts*/
					    G4bool(definition.globalTransform),
					    transform,
					    nullptr, /* no cavity info*/
					    magFile,
					    magFormat,
					    magIntType,
					    eleFile,
					    eleFormat,
					    eleIntType,
					    false,   /*don't cache transforms*/
					    G4double(definition.scaling));

      parserDefinitions[G4String(definition.name)] = info;
    }
}

BDSFieldInfo* BDSFieldFactory::GetDefinition(G4String name) const
{
  // Here we test if the string is empty and return nullptr. We do this so
  // this method can be used without exiting when no key is specified at all.
  // If a key is given and not found, then that requires the users attention.
  if (name.empty())
    {return nullptr;}
  auto result = parserDefinitions.find(name);
  if (result == parserDefinitions.end())
    {// not a valid key
      G4cerr << __METHOD_NAME__ << "\"" << name << "\" is not a valid field specifier" << G4endl;
      G4cout << "Defined field speicifiers are:" << G4endl;
      for (const auto& it : parserDefinitions)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  return result->second;
}

BDSFieldObjects* BDSFieldFactory::CreateField(BDSFieldInfo& info)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << info << G4endl;
#endif
  // Forward on to delegate functions for the main types of field
  // such as E, EM and Magnetic
  BDSFieldObjects* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::zero:
    case BDSFieldType::threed:
    case BDSFieldType::xy:
    case BDSFieldType::mokka:
    case BDSFieldType::solenoid:
    case BDSFieldType::dipole:
    case BDSFieldType::quadrupole:
    case BDSFieldType::sextupole:
    case BDSFieldType::octupole:
    case BDSFieldType::decapole:
    case BDSFieldType::multipole:
    case BDSFieldType::muonspoiler:
    case BDSFieldType::skewquadrupole:
    case BDSFieldType::skewsextupole:
    case BDSFieldType::skewoctupole:
    case BDSFieldType::skewdecapole:
      field = CreateFieldMag(info);
      break;
    case BDSFieldType::rfcavity:
      field = CreateFieldEM(info);
      break;
    case BDSFieldType::rf:
      field = CreateFieldE(info);
      break;
    case BDSFieldType::none:
      break; // leave as nullptr
    default:
      G4cerr << __METHOD_NAME__ << "not a valid field type \"" << info.FieldType() << "\"" << G4endl;
      return field;
      break; // this will return nullptr
    }
  return field;
}
      
BDSFieldObjects* BDSFieldFactory::CreateFieldMag(BDSFieldInfo& info)
{
  BDSMagnetStrength* strength = info.MagnetStrength();
  G4double brho               = info.BRho();
  BDSFieldMag* field          = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::zero:
      {break;}
    case BDSFieldType::threed:
    case BDSFieldType::xy:
    case BDSFieldType::mokka:
      {field = BDSFieldLoader::Instance()->LoadMagField(info); break;}
    case BDSFieldType::solenoid:
      {field = new BDSFieldMagSolenoid(strength, brho); break;}
    case BDSFieldType::dipole:
      {field = new BDSFieldMagSBend(strength, brho); break;}
    case BDSFieldType::quadrupole:
      {field = new BDSFieldMagQuadrupole(strength, brho); break;}
    case BDSFieldType::dipolequadrupole:
      {field = new BDSFieldMagDipoleQuadrupole(strength, brho); break;}
    case BDSFieldType::sextupole:
      {field = new BDSFieldMagSextupole(strength, brho); break;}
    case BDSFieldType::octupole:
      {field = new BDSFieldMagOctupole(strength, brho); break;}
    case BDSFieldType::decapole:
      {field = new BDSFieldMagDecapole(strength, brho); break;}
    case BDSFieldType::multipole:
      {field = new BDSFieldMagMultipole(strength, brho); break;}
    case BDSFieldType::muonspoiler:
      {field = new BDSFieldMagMuonSpoiler(strength, brho); break;}
    case BDSFieldType::skewquadrupole:
      {field = new BDSFieldMagSkew(new BDSFieldMagQuadrupole(strength, brho), CLHEP::halfpi); break;}
    case BDSFieldType::skewsextupole:
      {field = new BDSFieldMagSkew(new BDSFieldMagSextupole(strength, brho), CLHEP::pi/6.); break;}
    case BDSFieldType::skewoctupole:
      {field = new BDSFieldMagSkew(new BDSFieldMagOctupole(strength, brho), CLHEP::pi/8.); break;}
    case BDSFieldType::skewdecapole:
      {field = new BDSFieldMagSkew(new BDSFieldMagDecapole(strength, brho), CLHEP::pi/10.); break;}
    default:
      {
	return nullptr;
	break;
      }
    }
  
  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldMag* resultantField = field;
  if (info.ProvideGlobal())
    {resultantField = new BDSFieldMagGlobal(field);}

  // Set transform for local geometry offset
  resultantField->SetTransform(info.Transform());

  // Always this equation of motion for magnetic (only) fields
  G4Mag_UsualEqRhs* eqOfM = new G4Mag_UsualEqRhs(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorMag(info, eqOfM, strength);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateFieldEM(BDSFieldInfo& info)
{
  BDSFieldEM* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::rfcavity:
      field = new BDSFieldEMRFCavity(info.CavityInfo()); break;
    default:
      return nullptr;
      break;
    }

  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldEM* resultantField = field;
  if (info.ProvideGlobal())
    {resultantField = new BDSFieldEMGlobal(field);}

  // Set transform for local geometry offset
  resultantField->SetTransform(info.Transform());

  // Equation of motion for em fields
  G4EqMagElectricField* eqOfM = new G4EqMagElectricField(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorEM(info, eqOfM);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateFieldE(BDSFieldInfo& info)
{
  BDSFieldE* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::rf:
      field = new BDSFieldESinusoid(info.CavityInfo()); break;
    default:
      return nullptr;
      break;
    }

  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldE* resultantField = field;
  if (info.ProvideGlobal())
    {resultantField = new BDSFieldEGlobal(field);}

  // Set transform for local geometry offset
  resultantField->SetTransform(info.Transform());

  // Equation of motion for em fields
  G4EqMagElectricField* eqOfM = new G4EqMagElectricField(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorE(info, eqOfM);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorMag(BDSFieldInfo&      info,
							     G4Mag_EqRhs*       eqOfM,
							     BDSMagnetStrength* strength)
{
  G4double                      brho = info.BRho();
  G4MagIntegratorStepper* integrator = nullptr;
  // these ones can only be used for magnetic field
  switch (info.IntegratorType().underlying())
    {
    case BDSIntegratorType::solenoid:
      integrator = new BDSIntegratorSolenoid(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipole:
      integrator = new BDSIntegratorDipole(strength, brho, eqOfM); break;
    case BDSIntegratorType::quadrupole:
      integrator = new BDSIntegratorQuadrupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::sextupole:
      integrator = new BDSIntegratorSextupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::octupole:
      integrator = new BDSIntegratorOctupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::decapole:
      integrator = new BDSIntegratorDecapole(strength, brho, eqOfM); break;
    case BDSIntegratorType::multipole:
      integrator = new BDSIntegratorMultipole(strength, brho, eqOfM); break;
    case BDSIntegratorType::fringe:
      integrator = new BDSIntegratorFringefield(strength, brho, eqOfM); break;
    case BDSIntegratorType::g4constrk4:
      integrator = new G4ConstRK4(eqOfM); break;
    case BDSIntegratorType::g4exacthelixstepper:
      integrator = new G4ExactHelixStepper(eqOfM); break;
    case BDSIntegratorType::g4helixexpliciteuler:
      integrator = new G4HelixExplicitEuler(eqOfM); break;
    case BDSIntegratorType::g4helixheum:
      integrator = new G4HelixHeum(eqOfM); break;
    case BDSIntegratorType::g4heliximpliciteuler:
      integrator = new G4HelixImplicitEuler(eqOfM); break;
    case BDSIntegratorType::g4helixmixedstepper:
      integrator = new G4HelixMixedStepper(eqOfM); break;
    case BDSIntegratorType::g4helixsimplerunge:
      integrator = new G4HelixSimpleRunge(eqOfM); break;
    case BDSIntegratorType::g4nystromrk4:
      integrator = new G4NystromRK4(eqOfM); break;
    case BDSIntegratorType::g4rkg3stepper:
      integrator = new G4RKG3_Stepper(eqOfM); break;
    case BDSIntegratorType::g4cashkarprkf45:
    case BDSIntegratorType::g4classicalrk4:
    case BDSIntegratorType::g4expliciteuler:
    case BDSIntegratorType::g4impliciteuler:
    case BDSIntegratorType::g4simpleheum:
    case BDSIntegratorType::g4simplerunge:
      integrator = CreateIntegratorEM(info, (G4EquationOfMotion*)eqOfM); break;
    default:
      break; // returns nullptr;
    }
  
  return integrator;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorEM(BDSFieldInfo&       info,
							    G4EquationOfMotion* eqOfM)
{
  G4MagIntegratorStepper* integrator = nullptr;
  switch (info.IntegratorType().underlying())
    {
      // do the EM ones first, then complain
    case BDSIntegratorType::g4cashkarprkf45:
      integrator = new G4CashKarpRKF45(eqOfM); break;
    case BDSIntegratorType::g4classicalrk4:
      integrator = new G4ClassicalRK4(eqOfM); break;
    case BDSIntegratorType::g4expliciteuler:
      integrator = new G4ExplicitEuler(eqOfM); break;
    case BDSIntegratorType::g4impliciteuler:
      integrator = new G4ImplicitEuler(eqOfM); break;
    case BDSIntegratorType::g4simpleheum:
      integrator = new G4SimpleHeum(eqOfM); break;
    case BDSIntegratorType::g4simplerunge:
      integrator = new G4SimpleRunge(eqOfM); break;
    case BDSIntegratorType::solenoid:
    case BDSIntegratorType::dipole:
    case BDSIntegratorType::quadrupole:
    case BDSIntegratorType::sextupole:
    case BDSIntegratorType::octupole:
    case BDSIntegratorType::decapole:
    case BDSIntegratorType::fringe:
    case BDSIntegratorType::g4constrk4:
    case BDSIntegratorType::g4exacthelixstepper:
    case BDSIntegratorType::g4helixexpliciteuler:
    case BDSIntegratorType::g4helixheum:
    case BDSIntegratorType::g4heliximpliciteuler:
    case BDSIntegratorType::g4helixmixedstepper:
    case BDSIntegratorType::g4helixsimplerunge:
    case BDSIntegratorType::g4nystromrk4:
    case BDSIntegratorType::g4rkg3stepper:
      {
	G4cerr << "Error: integrator \"" << info.IntegratorType() << "\" is not suitable for an EM field." << G4endl;
	G4cout << "Suitable integrators are:" << G4endl;
	std::vector<BDSIntegratorType> types = {
	  BDSIntegratorType::g4cashkarprkf45,
	  BDSIntegratorType::g4classicalrk4,
	  BDSIntegratorType::g4expliciteuler,
	  BDSIntegratorType::g4impliciteuler,
	  BDSIntegratorType::g4simpleheum,
	  BDSIntegratorType::g4simplerunge};
	for (auto type : types)
	  {G4cout << type << G4endl;}
	exit(1);
      }
    default:
      break; // returns nullptr;
    }
  return integrator;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorE(BDSFieldInfo&       info,
							   G4EquationOfMotion* eqOfM)
{
  return CreateIntegratorEM(info,eqOfM);
}

BDSMagFieldMesh* BDSFieldFactory::CreateMagneticField(G4String      formatAndFilePath,
						      G4ThreeVector offsetIn,
						      BDSGeometry*  geometryIn)
  
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  offset            = offsetIn;
  geometry          = geometryIn;

  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFilePath);
  fileName = BDS::GetFullPath(ff.first);
  format   = BDS::DetermineFieldType(ff.second);
  /*
  if (format == BDSFieldType::threed)
    {return CreateMagField3D();}
  
  else if (format == BDSFieldType::xy)
    {return CreateMagFieldXY();}
  
  else if (format == BDSFieldType::zero)
    {
    if(geometry)
      {
	if (geometry->Format() == BDSGeometryType::lcdd)
	  {return CreateMagFieldLCDD();}
      }
    }

  // this logic seems unclear
  else if (format == BDSFieldType::mokka || format == BDSFieldType::zero)
    {
      if(geometry)
	{
	  if (geometry->Format() == BDSGeometryType::mokka)
	    {return CreateMagFieldSQL();}
	}
    }
  */
  return nullptr;
}

BDSFieldObjects* BDSFieldFactory::CreateTeleporter(G4ThreeVector teleporterDelta)
{
  bGlobalField = new BDSMagFieldMesh(); //Zero magnetic field.
  bEqOfMotion  = new G4Mag_UsualEqRhs(bGlobalField);
  integrator   = new BDSIntegratorTeleporter(bEqOfMotion, teleporterDelta);
  BDSFieldObjects* completeField = new BDSFieldObjects(nullptr, bGlobalField, bEqOfMotion, integrator);
  return completeField;
}


BDSFieldObjects* BDSFieldFactory::CreateFieldMagOuter(const BDSMagnetType      type,
						      BDSMagnetStrength* const /*strength*/,
						      const G4double           /*brho*/)
{
  // switch on the type and build correct field
  switch (type.underlying())
    {
    case BDSMagnetType::sectorbend:
    case BDSMagnetType::rectangularbend:
    case BDSMagnetType::quadrupole:
    case BDSMagnetType::sextupole:
    case BDSMagnetType::octupole:
    case BDSMagnetType::decapole:
    case BDSMagnetType::multipole:
      //CreateOuterMultipole(type, strength, brho); break;
    case BDSMagnetType::solenoid:
    case BDSMagnetType::vkicker:
    case BDSMagnetType::hkicker:
      break; // return a nullptr
    case BDSMagnetType::muonspoiler:
      //CreateMuonSpoiler(strength, brho); break;
    default:
      G4cerr << __METHOD_NAME__ << "no outer field defined for this type of magnet" << G4endl;
      break; // return a nullptr
    }
  return completeField;
}

/*
BDSMagFieldMesh* BDSFieldFactory::CreateMagFieldSQL()
{
  if(geometry->HasFields() || !fileName.empty())
    {
      // Check for field file or volumes with fields
      // as there may be cases where there are no formats given
      // in gmad file but fields might be set to volumes in SQL files
      return new BDSFieldMagSQL(fileName,
				geometry->Length(),
				geometry->QuadVolBgrad(),
				geometry->SextVolBgrad(),
				geometry->OctVolBgrad(),
				geometry->UniformFieldVolField(),
				geometry->NPoleField(),
				geometry->HasUniformField());
    }
  else
    {return nullptr;}
}


BDSMagFieldMesh* BDSFieldFactory::CreateMagFieldLCDD()
{return (BDSMagFieldMesh*)(geometry->Field());}
*/
