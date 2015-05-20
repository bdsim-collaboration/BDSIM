#include "BDSGeometryFactory.hh"
#include "BDSExecOptions.hh"
#include "ggmad.hh"
#include "BDSGeometryLCDD.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryGDML.hh"

BDSGeometryFactory::BDSGeometryFactory(){
  _gFormat=new BDSGeometryFormat();
}
BDSGeometryFactory::~BDSGeometryFactory(){}

BDSGeometry* BDSGeometryFactory::buildGeometry(G4String geometry=""){
  _geometry=geometry;
  parseFormatAndFilename();
  if (_gFormat->compare(G4String("gmad"))) {
    buildGmad();
  } 
  else if (_gFormat->compare(G4String("lcdd"))) {
    buildLCDD();
  }
  else if (_gFormat->compare(G4String("mokka"))) {
    buildMokka();
  }
  else if (_gFormat->compare(G4String("gdml"))) {
    buildGDML();
  }
  else if (_gFormat->compare(G4String("none"))) {
    buildNone();
  }
  else if (_gFormat->compare(G4String(""))) {
    buildNone();
  }
}

void BDSGeometryFactory::parseFormatAndFilename(){
  if(_geometry != ""){
    G4int pos = _geometry.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid geometry reference format : "<<_geometry<<G4endl;
    }
    else {
      _gFormat->spec(_geometry.substr(0,pos));
      _gFile = BDSExecOptions::Instance()->GetBDSIMPATH() + _geometry.substr(pos+1,_geometry.length() - pos); 
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - geometry file is " << _gFile << G4endl;
#endif
    }
  }
}

BDSGeometry* BDSGeometryFactory::buildGmad(){
  return new GGmadDriver(_gFile);
}

BDSGeometry* BDSGeometryFactory::buildLCDD(){
  return new BDSGeometryLCDD(_gFile);
}

BDSGeometry* BDSGeometryFactory::buildMokka(){
  return new BDSGeometrySQL(_gFile);
}

BDSGeometry* BDSGeometryFactory::buildGDML(){
  return new BDSGeometryGDML(_gFile);
}

BDSGeometry* BDSGeometryFactory::buildNone(){
  return NULL;
}



