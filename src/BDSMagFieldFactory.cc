#include "BDSMagFieldFactory.hh"
#include "BDSExecOptions.hh"
#include "BDSXYMagField.hh"
#include "BDS3DMagField.hh"
#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSMagFieldSQL.hh"
#include "G4UniformMagField.hh"
#include "BDSDebug.hh"
#include <typeinfo>

BDSMagFieldFactory::BDSMagFieldFactory()
{
  bFormat = new BDSBmapFormat();
  cacheLength = 1*CLHEP::um;
}
BDSMagFieldFactory::~BDSMagFieldFactory()
{
  delete bFormat;
}

BDSMagField* BDSMagFieldFactory::BuildMagneticField(G4String     bmapIn,
						    G4double     bmapZOffsetIn,
						    BDSGeometry* geometryIn,
						    G4double     bmapXOffsetIn)
{
  bmap        = bmapIn;
  bmapZOffset = bmapZOffsetIn;
  geometry    = geometryIn;
  bmapXOffset = bmapXOffsetIn;
  
  parseFormatAndFilename();
  if (bFormat->compare("3D"))
    {
      G4cout <<  __METHOD_NAME__ << " - building 3D mag field... " << G4endl;
      return Build3DMagField();
    } 
  else if (bFormat->compare("XY"))
    {
      return BuildXYMagField();
    }
  else if (bFormat->compare("none"))
    {
    if(geom)
      {
	if (geom->format()->compare(new BDSGeometryFormat("lcdd")))
	  {return BuildLCDDMagField();}
      }
    }
  else if (bFormat->compare("mokka") || bFormat->compare("none"))
    {
      if(geom)
	{
	  if (geom->format()->compare(new BDSGeometryFormat("mokka")))
	    {return BuildSQLMagField();}
	}
    }
  else
    {return nullptr;}
}

void BDSMagFieldFactory::parseFormatAndFilename(){
  if(bmap != ""){
    G4int pos = bmap.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid B map reference format : "<<bmap<<G4endl;
    }
    else {
      _bFormat->spec(bmap.substr(0,pos));
      _bFile = BDSExecOptions::Instance()->GetBDSIMPATH() + bmap.substr(pos+1,bmap.length() - pos); 
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - bmap file is " << _bFile << G4endl;
#endif
    }
  }
}

BDSMagField* BDSMagFieldFactory::buildXYMagField(){
  return new BDSXYMagField(_bFile);
}

BDSMagField* BDSMagFieldFactory::build3DMagField(){
  return new BDS3DMagField(_bFile, bmapZOffset, bmapXOffset);
}

BDSMagField* BDSMagFieldFactory::buildSQLMagField(){
  if(_geom->hasFields() || _bFile!=""){
    // Check for field file or volumes with fields
    // as there may be cases where there are no bFormats given
    // in gmad file but fields might be set to volumes in SQL files
    return new BDSMagFieldSQL(_bFile,_geom->length(),
			      _geom->quadVolBgrad(),
			      _geom->sextVolBgrad(),
			      _geom->octVolBgrad(),
			      _geom->uniformFieldVolField(),
			      _geom->nPoleField(),
			      _geom->hasUniformField());
  }
  else return buildZeroMagField();
}

BDSMagField* BDSMagFieldFactory::buildLCDDMagField()
{return (BDSMagField*)(geometry->field());}

BDSMagField* BDSMagFieldFactory::buildZeroMagField(){
  return new BDSMagField(); //Zero magnetic field.
}
