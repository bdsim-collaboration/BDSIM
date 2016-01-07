#include "BDSFieldType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldFactory.hh"
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
						     BDSMagnetStrength* strengths,
						     G4double           nominalrigidity)
{
  CleanUp();
  
  // switch on type and build correct field
  switch (type)
    {
    case BDSFieldType::solenoid:
      BuildFieldSolenoid(strengths,   nominalrigidity); break;
    case BDSFieldType::dipole:
      BuildFieldDipole(strengths,     nominalrigidity); break;
    case BDSFieldType::quadrupole:
      BuildFieldQuadrupole(strengths, nominalrigidity); break;
    case BDSFieldType::sextupole:
      BuildFieldSextupole(strengths,  nominalrigidity); break;
    case BDSFieldType::octupole:
      BuildFieldOctupole(strengths,   nominalrigidity); break;
    case BDSFieldType::decapole:
      BuildFieldDecapole(strengths,   nominalrigidity); break;
    case BDSFieldType::multipole:
      BuildFieldMultipole(strengths,  nominalrigidity); break;
    case BDSFieldType::sbend:
      BuildFieldSBend(strengths,      nominalrigidity); break;
    default:
      G4cerr << __METHOD_NAME__ << "not an equation based field type" << G4endl;
      exit(1);
      break;
    }
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CommonFieldConstruction(G4Field* field,
							  BDSIntegratorType integratorType)
{
  // 1 field
  // 2 equation of motion
  // 3 integrator
  // 4 chord finder
  // 5 field manager
  // 6 package it up
  
  // 2 equation of motion
  G4EquationOfMotion* eq = new G4Mag_UsualEqRhs(field);

  // 3 integrator
}

void BDSFieldFactory::BuildFieldSolenoid(BDSMagnetStrength* strengths,
					 G4double           nominalRigidity)
{
  field = new BDSQuad
  
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
