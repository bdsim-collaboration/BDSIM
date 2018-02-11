/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldClassType.hh"
#include "BDSFieldE.hh"
#include "BDSFieldEGlobal.hh"
#include "BDSFieldEInterpolated.hh"
#include "BDSFieldESinusoid.hh"
#include "BDSFieldEZero.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobal.hh"
#include "BDSFieldEMInterpolated.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSFieldEMZero.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagDecapole.hh"
#include "BDSFieldMagDipoleQuadrupole.hh"
#include "BDSFieldMagGlobal.hh"
#include "BDSFieldMagInterpolated.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSFieldMagOctupole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSFieldMagSkewOwn.hh"
#include "BDSFieldMagSolenoid.hh"
#include "BDSFieldMagZero.hh"
#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"
#include "BDSGeometryType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorDecapole.hh"
#include "BDSIntegratorDipoleRodrigues.hh"
#include "BDSIntegratorDipoleRodrigues2.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"
#include "BDSIntegratorEuler.hh"
#include "BDSIntegratorKickerThin.hh"
#include "BDSIntegratorOctupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSIntegratorMultipoleThin.hh"
#include "BDSIntegratorSextupole.hh"
#include "BDSIntegratorSolenoid.hh"
#include "BDSIntegratorTeleporter.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"
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
#include "G4Version.hh"

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
#if G4VERSION_NUMBER > 1029
#include "G4BogackiShampine23.hh"
#include "G4BogackiShampine45.hh"
#include "G4DoLoMcPriRK34.hh"
#include "G4DormandPrince745.hh"
#include "G4DormandPrinceRK56.hh"
#include "G4DormandPrinceRK78.hh"
#include "G4TsitourasRK45.hh"
#endif
#if G4VERSION_NUMBER > 1039
#include "G4RK547FEq1.hh"
#include "G4RK547FEq2.hh"
#include "G4RK547FEq3.hh"
#endif

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <limits>
#include <map>
#include <utility>
#include <vector>

G4double BDSFieldFactory::defaultRigidity = std::numeric_limits<double>::max();

BDSFieldFactory* BDSFieldFactory::instance = nullptr;

BDSFieldFactory* BDSFieldFactory::Instance()
{
  if (instance == nullptr)
    {instance = new BDSFieldFactory();}
  return instance;
}

BDSFieldFactory::BDSFieldFactory()
{
  PrepareFieldDefinitions(BDSParser::Instance()->GetFields(),
			  defaultRigidity);
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

      G4RotationMatrix rm;
      if (definition.axisAngle)
	{
	  G4ThreeVector axis = G4ThreeVector(definition.axisX,
					     definition.axisY,
					     definition.axisZ);
	  rm = G4RotationMatrix(axis, definition.angle*CLHEP::rad);
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
	      rm = G4RotationMatrix(ang);
	    }
	  // else rm is default rotation matrix
	}
      
      G4Transform3D transform = G4Transform3D(rm, offset);

      BDSFieldFormat magFormat = BDSFieldFormat::none;
      G4String       magFile   = "";
      G4bool  magFileSpecified = !definition.magneticFile.empty();
      if (magFileSpecified)
	{
	  std::pair<G4String, G4String> bf = BDS::SplitOnColon(G4String(definition.magneticFile));
	  magFormat = BDS::DetermineFieldFormat(bf.first);
	  magFile   = BDS::GetFullPath(bf.second);
	}
      
      BDSFieldFormat eleFormat = BDSFieldFormat::none;
      G4String       eleFile   = "";
      G4bool  eleFileSpecified = !definition.electricFile.empty();
      if (eleFileSpecified)
	{
	  std::pair<G4String, G4String> ef = BDS::SplitOnColon(G4String(definition.electricFile));
	  eleFormat = BDS::DetermineFieldFormat(ef.first);
	  eleFile   = BDS::GetFullPath(ef.second);
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
					    nullptr, /*for now, no parameterised strengths*/
					    G4bool(definition.globalTransform),
					    transform,
					    magFile,
					    magFormat,
					    magIntType,
					    eleFile,
					    eleFormat,
					    eleIntType,
					    false,   /*don't cache transforms*/
					    G4double(definition.eScaling),
					    G4double(definition.bScaling),
					    G4double(definition.t*CLHEP::s),
					    G4bool(definition.autoScale));

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
      G4cout << "Defined field specifiers are:" << G4endl;
      for (const auto& it : parserDefinitions)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  return result->second;
}

BDSFieldObjects* BDSFieldFactory::CreateField(const BDSFieldInfo&      info,
					      const BDSMagnetStrength* scalingStrength,
					      const G4String           scalingKey)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << info << G4endl;
#endif
  // Forward on to delegate functions for the main types of field
  // such as E, EM and Magnetic
  BDSFieldObjects* field = nullptr;
  
  if (info.FieldType() == BDSFieldType::none)
    {return field;} // as nullptr

  BDSFieldClassType clas = BDS::DetermineFieldClassType(info.FieldType());
  switch (clas.underlying())
    {
    case BDSFieldClassType::magnetic:
      {field = CreateFieldMag(info, scalingStrength, scalingKey); break;}
    case BDSFieldClassType::electromagnetic:
      {field = CreateFieldEM(info); break;}
    case BDSFieldClassType::electric:
      {field = CreateFieldE(info); break;}
    case BDSFieldClassType::irregular:
      {field = CreateFieldIrregular(info); break;}
    default:
      {break;} // this will return nullptr
    }
  return field;
}
      
BDSFieldObjects* BDSFieldFactory::CreateFieldMag(const BDSFieldInfo&      info,
						 const BDSMagnetStrength* scalingStrength,
						 const G4String           scalingKey)
{
  const BDSMagnetStrength* strength = info.MagnetStrength();
  G4double brho               = info.BRho();
  BDSFieldMag* field          = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::bmap1d:
    case BDSFieldType::bmap2d:
    case BDSFieldType::bmap3d:
    case BDSFieldType::bmap4d:
    case BDSFieldType::mokka:
      {
	field = BDSFieldLoader::Instance()->LoadMagField(info,
							 scalingStrength,
							 scalingKey);
	break;
      }
    case BDSFieldType::bzero:
      {field = new BDSFieldMagZero(); break;}
    case BDSFieldType::solenoid:
      {field = new BDSFieldMagSolenoid(strength, brho); break;}
    case BDSFieldType::dipole:
      {
	G4ThreeVector unitDirection = G4ThreeVector(0,1,0);
	field = new BDSFieldMagDipole(strength, brho, unitDirection);
	break;
      }
    case BDSFieldType::dipole3d:
      {
	G4ThreeVector unitDirection = G4ThreeVector((*strength)["bx"],
						    (*strength)["by"],
						    (*strength)["bz"]);
	unitDirection = unitDirection.unit(); // ensure unit vector
	field = new BDSFieldMagDipole(strength, brho, unitDirection);
	break;
      }
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
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagQuadrupole(strength, brho), CLHEP::pi/4.); break;}
    case BDSFieldType::skewsextupole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagSextupole(strength, brho), CLHEP::pi/6.); break;}
    case BDSFieldType::skewoctupole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagOctupole(strength, brho), CLHEP::pi/8.); break;}
    case BDSFieldType::skewdecapole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagDecapole(strength, brho), CLHEP::pi/10.); break;}
    default:
      {// there is no need for case BDSFieldType::none as this won't be used in this function.
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
  BDSMagUsualEqRhs* eqOfM = new BDSMagUsualEqRhs(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorMag(info, eqOfM, strength);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateFieldEM(const BDSFieldInfo& info)
{
  BDSFieldEM* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::rfcavity:
      {field = new BDSFieldEMRFCavity(info.MagnetStrength(), info.BRho()); break;}
    case BDSFieldType::ebmap1d:
    case BDSFieldType::ebmap2d:
    case BDSFieldType::ebmap3d:
    case BDSFieldType::ebmap4d:
      {field = BDSFieldLoader::Instance()->LoadEMField(info); break;}
    case BDSFieldType::ebzero:
      {field = new BDSFieldEMZero(); break;}
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

BDSFieldObjects* BDSFieldFactory::CreateFieldE(const BDSFieldInfo& info)
{
  BDSFieldE* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::rf:
      {field = new BDSFieldESinusoid(info.MagnetStrength(), info.BRho()); break;}
    case BDSFieldType::emap1d:
    case BDSFieldType::emap2d:
    case BDSFieldType::emap3d:
    case BDSFieldType::emap4d:
      {field = BDSFieldLoader::Instance()->LoadEField(info); break;}
    case BDSFieldType::ezero:
      {field = new BDSFieldEZero(); break;}
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

BDSFieldObjects* BDSFieldFactory::CreateFieldIrregular(const BDSFieldInfo& info)
{
  // special routine for each special / irregular field
  BDSFieldObjects* result = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::teleporter:
      {result = CreateTeleporter(info); break;}
    default:
      {break;}
    }
  return result;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorMag(const BDSFieldInfo&      info,
							     G4Mag_EqRhs*             eqOfM,
							     const BDSMagnetStrength* strength)
{
  const G4double minimumRadiusOfCurvature = 10*CLHEP::cm;
  G4double                      brho = info.BRho();
  G4MagIntegratorStepper* integrator = nullptr;
  // these ones can only be used for magnetic field
  switch (info.IntegratorType().underlying())
    {
    case BDSIntegratorType::solenoid:
      integrator = new BDSIntegratorSolenoid(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipolerodrigues:
      integrator = new BDSIntegratorDipoleRodrigues(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipolerodrigues2:
      integrator = new BDSIntegratorDipoleRodrigues2(eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::dipolematrix:
      integrator = new BDSIntegratorDipoleQuadrupole(strength, brho, eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::quadrupole:
      integrator = new BDSIntegratorQuadrupole(strength, brho, eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::sextupole:
      integrator = new BDSIntegratorSextupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::octupole:
      integrator = new BDSIntegratorOctupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::decapole:
      integrator = new BDSIntegratorDecapole(strength, brho, eqOfM); break;
    case BDSIntegratorType::multipolethin:
      integrator = new BDSIntegratorMultipoleThin(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipolefringe:
      integrator = new BDSIntegratorDipoleFringe(strength, brho, eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::euler:
      integrator = new BDSIntegratorEuler(eqOfM); break;
    case BDSIntegratorType::kickerthin:
      integrator = new BDSIntegratorKickerThin(strength, brho, eqOfM); break;
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
#if G4VERSION_NUMBER > 1029
    case BDSIntegratorType::g4bogackishampine23:
    case BDSIntegratorType::g4bogackishampine45:
    case BDSIntegratorType::g4dolomcprirk34:
    case BDSIntegratorType::g4dormandprince745:
    case BDSIntegratorType::g4dormandprincerk56:
    case BDSIntegratorType::g4dormandprincerk78:
    case BDSIntegratorType::g4tsitourasrk45:
#endif
#if G4VERSION_NUMBER > 1039
    case BDSIntegratorType::g4rk547feq1:
    case BDSIntegratorType::g4rk547feq2:
    case BDSIntegratorType::g4rk547feq3:
#endif
      integrator = CreateIntegratorEM(info, (G4EquationOfMotion*)eqOfM); break;
    default:
      break; // returns nullptr;
    }
  
  return integrator;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorEM(const BDSFieldInfo& info,
							    G4EquationOfMotion* eqOfM)
{
  G4MagIntegratorStepper* integrator = nullptr;
  switch (info.IntegratorType().underlying())
    {
      // do the EM ones first, then complain
    case BDSIntegratorType::g4cashkarprkf45:
      integrator = new G4CashKarpRKF45(eqOfM, 8); break;
    case BDSIntegratorType::g4classicalrk4:
      integrator = new G4ClassicalRK4(eqOfM, 8); break;
    case BDSIntegratorType::g4expliciteuler:
      integrator = new G4ExplicitEuler(eqOfM, 8); break;
    case BDSIntegratorType::g4impliciteuler:
      integrator = new G4ImplicitEuler(eqOfM, 8); break;
    case BDSIntegratorType::g4simpleheum:
      integrator = new G4SimpleHeum(eqOfM, 8); break;
    case BDSIntegratorType::g4simplerunge:
      integrator = new G4SimpleRunge(eqOfM, 8); break;
#if G4VERSION_NUMBER > 1029
    case BDSIntegratorType::g4bogackishampine23:
      {integrator = new G4BogackiShampine45(eqOfM, 8); break;}
    case BDSIntegratorType::g4bogackishampine45:
      {integrator = new G4BogackiShampine45(eqOfM, 8); break;}
    case BDSIntegratorType::g4dolomcprirk34:
      {integrator = new G4DoLoMcPriRK34(eqOfM, 8); break;}
    case BDSIntegratorType::g4dormandprince745:
      {integrator = new G4DormandPrince745(eqOfM, 8); break;}
    case BDSIntegratorType::g4dormandprincerk56:
      {integrator = new G4DormandPrinceRK56(eqOfM, 8); break;}
    case BDSIntegratorType::g4dormandprincerk78:
      {integrator = new G4DormandPrinceRK78(eqOfM, 8); break;}
    case BDSIntegratorType::g4tsitourasrk45:
      {integrator = new G4TsitourasRK45(eqOfM, 8); break;}
#endif
#if G4VERSION_NUMBER > 1039
    case BDSIntegratorType::g4rk547feq1:
      {integrator = new G4RK547FEq1(eqOfM, 8); break;}
    case BDSIntegratorType::g4rk547feq2:
      {integrator = new G4RK547FEq2(eqOfM, 8); break;}
    case BDSIntegratorType::g4rk547feq3:
      {integrator = new G4RK547FEq3(eqOfM, 8); break;}
#endif
    case BDSIntegratorType::solenoid:
    case BDSIntegratorType::dipolerodrigues:
    case BDSIntegratorType::quadrupole:
    case BDSIntegratorType::sextupole:
    case BDSIntegratorType::octupole:
    case BDSIntegratorType::decapole:
    case BDSIntegratorType::dipolefringe:
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
	  BDSIntegratorType::g4simplerunge
#if G4VERSION_NUMBER > 1029
	  ,
	  BDSIntegratorType::g4bogackishampine23,
	  BDSIntegratorType::g4bogackishampine45,
	  BDSIntegratorType::g4dolomcprirk34,
	  BDSIntegratorType::g4dormandprince745,
	  BDSIntegratorType::g4dormandprincerk56,
	  BDSIntegratorType::g4dormandprincerk78,
	  BDSIntegratorType::g4tsitourasrk45
#endif
#if G4VERSION_NUMBER > 1039
	  ,
	  BDSIntegratorType::g4rk547feq1,
	  BDSIntegratorType::g4rk547feq2,
	  BDSIntegratorType::g4rk547feq3
#endif
	};
	for (auto type : types)
	  {G4cout << type << G4endl;}
	exit(1);
      }
    default:
      break; // returns nullptr;
    }
  return integrator;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorE(const BDSFieldInfo& info,
							   G4EquationOfMotion* eqOfM)
{
  return CreateIntegratorEM(info,eqOfM);
}

BDSFieldObjects* BDSFieldFactory::CreateTeleporter(const BDSFieldInfo& info)
{
  const G4ThreeVector teleporterDelta = info.Transform().getTranslation();
  G4MagneticField* bGlobalField       = new BDSFieldMagZero();
  G4Mag_EqRhs*     bEqOfMotion        = new G4Mag_UsualEqRhs(bGlobalField);
  G4MagIntegratorStepper* integrator  = new BDSIntegratorTeleporter(bEqOfMotion, teleporterDelta);
  BDSFieldObjects* completeField      = new BDSFieldObjects(nullptr, bGlobalField,
							    bEqOfMotion, integrator);
  return completeField;
}
