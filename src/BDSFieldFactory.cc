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
#include "BDSFieldMagDecapole.hh"
#include "BDSFieldMagGlobal.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSFieldMagOctupole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagSBend.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSFieldMagSkew.hh"
#include "BDSFieldMagSolenoid.hh"
#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"

#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryType.hh"

#include "BDSIntegratorDecapole.hh"
#include "BDSIntegratorDipole.hh"
#include "BDSIntegratorOctupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSIntegratorSextupole.hh"
#include "BDSIntegratorSolenoid.hh"
#include "BDSIntegratorTeleporter.hh"
#include "BDSIntegratorType.hh"

#include "BDSMagFieldMesh.hh"
#include "BDSMagFieldSQL.hh"
#include "BDSMagFieldXY.hh"
#include "BDSMagField3D.hh"
#include "BDSMagnetType.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
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

#include <typeinfo>
#include <utility>

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
    default:
      G4cerr << __METHOD_NAME__ << "not a valid field type" << G4endl;
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
      break;
    case BDSFieldType::threed:
      /*field = new BDSFieldMag3D(filename, info.transform);*/ break;
    case BDSFieldType::xy:
      break;
    case BDSFieldType::mokka:
      break;
    case BDSFieldType::solenoid:
      field = new BDSFieldMagSolenoid(strength, brho); break;
    case BDSFieldType::dipole:
      field = new BDSFieldMagSBend(strength, brho); break;
    case BDSFieldType::quadrupole:
      field = new BDSFieldMagQuadrupole(strength, brho); break;
    case BDSFieldType::sextupole:
      field = new BDSFieldMagSextupole(strength, brho); break;
    case BDSFieldType::octupole:
      field = new BDSFieldMagOctupole(strength, brho); break;
    case BDSFieldType::decapole:
      field = new BDSFieldMagDecapole(strength, brho); break;
    case BDSFieldType::multipole:
      field = new BDSFieldMagMultipole(strength, brho); break;
    case BDSFieldType::muonspoiler:
      field = new BDSFieldMagMuonSpoiler(strength, brho); break;
    case BDSFieldType::skewquadrupole:
      field = new BDSFieldMagSkew(new BDSFieldMagQuadrupole(strength, brho), CLHEP::halfpi); break;
    case BDSFieldType::skewsextupole:
      field = new BDSFieldMagSkew(new BDSFieldMagSextupole(strength, brho), CLHEP::pi/6.); break;
    case BDSFieldType::skewoctupole:
      field = new BDSFieldMagSkew(new BDSFieldMagOctupole(strength, brho), CLHEP::pi/8.); break;
    case BDSFieldType::skewdecapole:
      field = new BDSFieldMagSkew(new BDSFieldMagDecapole(strength, brho), CLHEP::pi/10.); break;
    default:
      break; // this will return nullptr
    }

  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldMag* resultantField = field;
  if (info.ProvideGlobal())
    {resultantField = new BDSFieldMagGlobal(field);}

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
      break;
    }

  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldEM* resultantField = field;
  if (info.ProvideGlobal())
    {resultantField = new BDSFieldEMGlobal(field);}

  // Always this equation of motion for magnetic (only) fields
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
      break;
    }

  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldE* resultantField = field;
  if (info.ProvideGlobal())
    {resultantField = new BDSFieldEGlobal(field);}

  //eqom
  // complete field

  BDSFieldObjects* completeField = nullptr;
  return completeField;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorMag(BDSFieldInfo&      info,
							     G4Mag_EqRhs*       eqOfM,
							     BDSMagnetStrength* strength)
{
  G4double brho = info.BRho();
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

BDSMagFieldMesh* BDSFieldFactory::CreateMagFieldXY()
{return new BDSMagFieldXY(fileName);}

BDSMagFieldMesh* BDSFieldFactory::CreateMagField3D()
{return new BDSMagField3D(fileName, offset);}

BDSMagFieldMesh* BDSFieldFactory::CreateMagFieldSQL()
{
  if(geometry->HasFields() || !fileName.empty())
    {
      // Check for field file or volumes with fields
      // as there may be cases where there are no formats given
      // in gmad file but fields might be set to volumes in SQL files
      return new BDSMagFieldSQL(fileName,
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
