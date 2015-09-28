#include "BDSMagFieldFactory.hh"
#include "BDSExecOptions.hh"
#include "BDSBType.hh"
#include "BDSMagFieldMesh.hh"
#include "BDSMagFieldXY.hh"
#include "BDSMagField3D.hh"
#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSMagFieldSQL.hh"
#include "G4UniformMagField.hh"
#include "BDSDebug.hh"
#include <typeinfo>

BDSMagFieldFactory::BDSMagFieldFactory()
{
  formatAndFilePath = "";
  offset            = G4ThreeVector(0,0,0);
  format            = BDSBType::zero;
  fileName          = "";
  geometry          = nullptr;
  cacheLength       = 1*CLHEP::um;
}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagneticField(G4String      formatAndFilePathIn,
							G4ThreeVector offsetIn,
							BDSGeometry*  geometryIn)
  
{
  formatAndFilePath = formatAndFilePathIn;
  offset            = offsetIn;
  geometry          = geometryIn;
  
  ParseFormatAndFilename();
  
  if (format == BDSBType::threed)
    {return BuildMagField3D();}
  
  else if (format == BDSBType::xy)
    {return BuildMagFieldXY();}
  
  else if (format == BDSBType::zero)
    {
    if(geometry)
      {
	if (geometry->format() == BDSGeometryType::lcdd)
	  {return BuildMagFieldLCDD();}
      }
    }
  
  else if (format == BDSBType::mokka || format == BDSBType::zero)
    {
      if(geometry)
	{
	  if (geometry->format() == BDSGeometryType::mokka)
	    {return BuildMagFieldSQL();}
	}
    }
  return nullptr;
}

void BDSMagFieldFactory::ParseFormatAndFilename()
{
  G4String s = G4String(formatAndFilePath); // shortcut
  if(!s.empty())
    {
      std::size_t found = s.find(":");
      if (found == std::string::npos)
	{
	  G4cerr << __METHOD_NAME__ << "ERROR: invalid B Field specifier \""
		 << s << "\" - missing \":\"" << G4endl;
	  exit(1);
	}
      else
	{
	  G4String formatName = s.substr(0,found);
	  format    = BDS::DetermineBType(formatName);
	  // TO BE FIXED - this only works with bdsim path
	  fileName  = BDSExecOptions::Instance()->GetBDSIMPATH();
	  fileName += s.substr(found+1); // get everything after ":"
	  
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << "format: " << format   << G4endl;
	G4cout << __METHOD_NAME__ << "file:   " << fileName << G4endl;
#endif
      }
    }
}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagFieldXY()
{return new BDSMagFieldXY(fileName);}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagField3D()
{return new BDSMagField3D(fileName, offset);}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagFieldSQL()
{
  if(geometry->hasFields() || !fileName.empty())
    {
      // Check for field file or volumes with fields
      // as there may be cases where there are no formats given
      // in gmad file but fields might be set to volumes in SQL files
      return new BDSMagFieldSQL(fileName,
				geometry->length(),
				geometry->quadVolBgrad(),
				geometry->sextVolBgrad(),
				geometry->octVolBgrad(),
				geometry->uniformFieldVolField(),
				geometry->nPoleField(),
				geometry->hasUniformField());
    }
  else
    {return nullptr;}
}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagFieldLCDD()
{return (BDSMagFieldMesh*)(geometry->field());}
