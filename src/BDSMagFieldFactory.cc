#include "BDSBType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSMagFieldFactory.hh"
#include "BDSMagFieldMesh.hh"
#include "BDSMagFieldSQL.hh"
#include "BDSMagFieldXY.hh"
#include "BDSMagField3D.hh"
#include "BDSUtilities.hh"

#include "G4UniformMagField.hh"

#include <typeinfo>
#include <utility>

BDSMagFieldFactory::BDSMagFieldFactory()
{
  offset            = G4ThreeVector(0,0,0);
  format            = BDSBType::zero;
  fileName          = "";
  geometry          = nullptr;
  cacheLength       = 1*CLHEP::um;
}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagneticField(G4String      formatAndFilePath,
							G4ThreeVector offsetIn,
							BDSGeometry*  geometryIn)
  
{
  offset            = offsetIn;
  geometry          = geometryIn;

  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFilePath);
  fileName = BDS::GetFullPath(ff.first);
  format   = BDS::DetermineBType(ff.second);
  
  if (format == BDSBType::threed)
    {return BuildMagField3D();}
  
  else if (format == BDSBType::xy)
    {return BuildMagFieldXY();}
  
  else if (format == BDSBType::zero)
    {
    if(geometry)
      {
	if (geometry->Format() == BDSGeometryType::lcdd)
	  {return BuildMagFieldLCDD();}
      }
    }

  // this logic seems unclear
  else if (format == BDSBType::mokka || format == BDSBType::zero)
    {
      if(geometry)
	{
	  if (geometry->Format() == BDSGeometryType::mokka)
	    {return BuildMagFieldSQL();}
	}
    }
  return nullptr;
}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagFieldXY()
{return new BDSMagFieldXY(fileName);}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagField3D()
{return new BDSMagField3D(fileName, offset);}

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagFieldSQL()
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

BDSMagFieldMesh* BDSMagFieldFactory::BuildMagFieldLCDD()
{return (BDSMagFieldMesh*)(geometry->Field());}
