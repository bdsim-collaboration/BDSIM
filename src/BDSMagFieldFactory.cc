#include "BDSMagFieldFactory.hh"
#include "BDSExecOptions.hh"
#include "BDSXYMagField.hh"
#include "BDS3DMagField.hh"

BDSMagFieldFactory::BDSMagFieldFactory(){
  _bFormat=new BDSBmapFormat();
}
BDSMagFieldFactory::~BDSMagFieldFactory(){}

BDSMagField* BDSMagFieldFactory::buildMagField(G4String bmap="", G4double bmapZOffset=0){
  _bmap=bmap;
  _bmapZOffset=bmapZOffset;
  parseFormatAndFilename();
  if (_bFormat->compare(G4String("3D"))) {
    return build3DMagField();
  } 
  else if (_bFormat->compare(G4String("XY"))){
    return buildXYMagField();
  } else {
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

BDSMagField* BDSMagFieldFactory::buildXYMagField(){
  return new BDSXYMagField(_bFile);
}

BDSMagField* BDSMagFieldFactory::build3DMagField(){
  return new BDS3DMagField(_bFile, _bmapZOffset);
}

BDSMagField* BDSMagFieldFactory::buildZeroMagField(){
  return new BDSMagField(); //Zero magnetic field.
}
