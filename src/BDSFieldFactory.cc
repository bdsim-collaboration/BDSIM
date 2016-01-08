#include "BDSFieldType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldMagSolenoid.hh"
#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSMagFieldMesh.hh"
#include "BDSMagFieldSQL.hh"
#include "BDSMagFieldXY.hh"
#include "BDSMagField3D.hh"
#include "BDSUtilities.hh"

#include "G4UniformMagField.hh"

#include <typeinfo>
#include <utility>


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

  CleanUp();
}

void BDSFieldType::CleanUp()
{
  field         = nullptr;
  integrator    = nullptr;
  completeField = nullptr;
}

BDSMagFieldMesh* BDSFieldFactory::BuildMagneticField(G4String      formatAndFilePath,
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
  format   = BDS::DetermineBType(ff.second);
  
  if (format == BDSFieldType::threed)
    {return BuildMagField3D();}
  
  else if (format == BDSFieldType::xy)
    {return BuildMagFieldXY();}
  
  else if (format == BDSFieldType::zero)
    {
    if(geometry)
      {
	if (geometry->Format() == BDSGeometryType::lcdd)
	  {return BuildMagFieldLCDD();}
      }
    }

  // this logic seems unclear
  else if (format == BDSFieldType::mokka || format == BDSFieldType::zero)
    {
      if(geometry)
	{
	  if (geometry->Format() == BDSGeometryType::mokka)
	    {return BuildMagFieldSQL();}
	}
    }
  return nullptr;
}

BDSFieldObjects* BDSFieldFactory::BuildFieldEquation(BDSFieldType       type,
						     BDSMagnetStrength* strength,
						     G4double           brho)
{
  CleanUp();
  
  // switch on type and build correct field
  switch (type)
    {
    case BDSFieldType::solenoid:
      BuildSolenoid(strength,   brho); break;
    case BDSFieldType::dipole:
      BuildDipole(strength,     brho); break;
    case BDSFieldType::quadrupole:
      BuildQuadrupole(strength, brho); break;
    case BDSFieldType::sextupole:
      BuildSextupole(strength,  brho); break;
    case BDSFieldType::octupole:
      BuildOctupole(strength,   brho); break;
    case BDSFieldType::decapole:
      BuildDecapole(strength,   brho); break;
    case BDSFieldType::multipole:
      BuildMultipole(strength,  brho); break;
    default:
      G4cerr << __METHOD_NAME__ << "not an equation based field type" << G4endl;
      exit(1);
      break;
    }
  return completeField;
}

void BDSFieldObjects::CommonConstructor()
{
  completeField = new BDSFieldObjects(field, eqOfM, integrator);
}

void BDSFieldFactory::BuildSolenoid(BDSMagnetStrength* strength,
				    G4double           brho)
{
  field         = new BDSFieldMagSolenoid(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorSolenoid(strength, brho);
  CommonConstructor();
}

void BDSFieldFactory::BuildDipole(BDSMagnetStrength* strength,
				  G4double           brho)
{
  field         = new BDSFieldMagSBend(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorSolenoid(strength, brho);
  CommonConstructor();
}

void BDSFieldFactory::BuildQuadrupole(BDSMagnetStrength* strength,
				      G4double           brho)
{
  field         = new BDSFieldMagQuadrupole(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorQuadrupole(strength, brho);
  CommonConstructor();
}

void BDSFieldFactory::BuildSextupole(BDSMagnetStrength* strength,
					  G4double           brho)
{
  field         = new BDSFieldMagSextupole(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorSextupole(strength, brho);
  CommonConstructor();
}

void BDSFieldFactory::BuildOctupole(BDSMagnetStrength* strength,
				    G4double           brho)
{
  field         = new BDSFieldMagOctupole(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorOctupole(strength, brho);
  CommonConstructor();
}

void BDSFieldFactory::BuildDecapole(BDSMagnetStrength* strength,
				    G4double           brho)
{
  field         = new BDSFieldMagDecapole(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorDecapole(strength, brho);
  CommonConstructor();
}

void BDSFieldFactory::BuildMultipole(BDSMagnetStrength* strength,
				     G4double           brho)
{
  field         = new BDSFieldMagMultipole(strength, brho);
  eqOfM         = new G4Mag_UsualEqRhs(field);
  integrator    = new BDSIntegratorMultipole(strength, brho);
  CommonConstructor();
}

BDSMagFieldMesh* BDSFieldFactory::BuildMagFieldXY()
{return new BDSMagFieldXY(fileName);}

BDSMagFieldMesh* BDSFieldFactory::BuildMagField3D()
{return new BDSMagField3D(fileName, offset);}

BDSMagFieldMesh* BDSFieldFactory::BuildMagFieldSQL()
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

BDSMagFieldMesh* BDSFieldFactory::BuildMagFieldLCDD()
{return (BDSMagFieldMesh*)(geometry->Field());}
