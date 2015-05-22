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

BDSMagFieldFactory::BDSMagFieldFactory(){
  _bFormat=new BDSBmapFormat();
  _cacheLength=1*CLHEP::um;
}
BDSMagFieldFactory::~BDSMagFieldFactory(){}


BDSMagField* BDSMagFieldFactory::buildMagField(G4String bmap="", G4double bmapZOffset=0, BDSGeometry* geom=NULL)
{
  _bmap=bmap;
  _bmapZOffset=bmapZOffset;
  _geom=geom;
  
  parseFormatAndFilename();
  if (_bFormat->compare("3D")) {
    G4cout <<  __METHOD_NAME__ << " - building 3D mag field... " << G4endl;
    return build3DMagField();
  } 
  else if (_bFormat->compare("XY")){
    return buildXYMagField();
  } 
  else if (_bFormat->compare("none")){
    if(_geom){
      if (_geom->format()->compare(new BDSGeometryFormat("lcdd"))){
	return buildLCDDMagField();
      }
    }
  }
  else if (_bFormat->compare("mokka") || _bFormat->compare("none")){
    if(_geom){
      if (_geom->format()->compare(new BDSGeometryFormat("mokka"))){
	return buildSQLMagField();
      } 
    }
  }
  //If none of the above, return zero magnetic field.
  return buildZeroMagField();
}

void BDSMagFieldFactory::parseFormatAndFilename(){
  if(_bmap != ""){
    G4int pos = _bmap.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid B map reference format : "<<_bmap<<G4endl;
    }
    else {
      _bFormat->spec(_bmap.substr(0,pos));
      _bFile = BDSExecOptions::Instance()->GetBDSIMPATH() + _bmap.substr(pos+1,_bmap.length() - pos); 
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
  return new BDS3DMagField(_bFile, _bmapZOffset);
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

BDSMagField* BDSMagFieldFactory::buildLCDDMagField(){
  return (BDSMagField*)_geom->field();
}

BDSMagField* BDSMagFieldFactory::buildZeroMagField(){
  return new BDSMagField(); //Zero magnetic field.
}
