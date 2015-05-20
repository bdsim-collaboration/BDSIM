#include "BDSMagFieldFactory.hh"
#include "BDSExecOptions.hh"
#include "BDSXYMagField.hh"
#include "BDS3DMagField.hh"
#include "BDSGeometry.hh"
#include "BDSGeometrySQL.hh"
#include "BDSMagFieldSQL.hh"
#include "G4UniformMagField.hh"
#include <typeinfo>

BDSMagFieldFactory::BDSMagFieldFactory(){
  _bFormat=new BDSBmapFormat();
  _cacheLength=1*CLHEP::um;
}
BDSMagFieldFactory::~BDSMagFieldFactory(){}


G4MagneticField* BDSMagFieldFactory::buildMagField(G4String bmap="", G4double bmapZOffset=0, BDSGeometry* geom=NULL)
{
  _bmap=bmap;
  _bmapZOffset=bmapZOffset;
  _geom=geom;

  parseFormatAndFilename();
  if (_bFormat->compare(G4String("3D"))) {
    return build3DMagField();
  } 
  else if (_bFormat->compare(G4String("XY"))){
    return buildXYMagField();
  } 
  else if (_bFormat->compare(G4String("none"))){
    if (geom->format()->compare("lcdd")){
      buildLCDDMagField();
    }
  }
  else if (_bFormat->compare(G4String("mokka")) || _bFormat->compare(G4String("none"))){
    if (geom->format()->compare("mokka")){
      buildSQLMagField();
    } 
  }

else {
    return buildZeroMagField();
  }
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

G4MagneticField* BDSMagFieldFactory::buildXYMagField(){
  return new BDSXYMagField(_bFile);
}

G4MagneticField* BDSMagFieldFactory::build3DMagField(){
  return new BDS3DMagField(_bFile, _bmapZOffset);
}

G4MagneticField* BDSMagFieldFactory::buildSQLMagField(){
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

G4MagneticField* BDSMagFieldFactory::buildLCDDMagField(){
  if(typeid(_geom->field()) == typeid(G4UniformMagField(0,0,0))){
    return _geom->field();
  } else {
    return _geom->field();
  }
}

G4MagneticField* BDSMagFieldFactory::buildZeroMagField(){
  return new BDSMagField(); //Zero magnetic field.
}
