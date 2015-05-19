#include "BDSMagFieldFactory.hh"
#include "BDSExecOptions.hh"
#include "BDSXYMagField.hh"
#include "BDS3DMagField.hh"

BDSMagFieldFactory::BDSMagFieldFactory(){}
BDSMagFieldFactory::~BDSMagFieldFactory(){}

BDSMagField* BDSMagFieldFactory::buildMagField(G4String bmap, G4double bmapZOffset){
  _bmap=bmap;
  _bmapZOffset=bmapZOffset;
  
  parseFormatAndFilename();
  if (_bFormat == "3D") {
    return build3DMagField();
  } 
  else if (_bFormat == "XY" ){
    return buildXYMagField();
  } else {

  }
}

void BDSMagFieldFactory::parseFormatAndFilename(){
  _bFormat="none";
  if(_bmap != ""){
    G4int pos = _bmap.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid B map reference format : "<<_bmap<<G4endl;
    }
    else {
      _bFormat = _bmap.substr(0,pos);
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


