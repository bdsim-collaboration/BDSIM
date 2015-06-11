#include "BDSGeometryFactory.hh"
#include "BDSExecOptions.hh"
#include "ggmad.hh"
#include "BDSGeometryLCDD.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryGDML.hh"
#include "BDSDebug.hh"


BDSGeometryFactory::BDSGeometryFactory(){
  _gFormat=new BDSGeometryFormat();
}
BDSGeometryFactory::~BDSGeometryFactory(){}

BDSGeometry* BDSGeometryFactory::buildGeometry(G4String geometry=""){
  _geometry=geometry;
  parseFormatAndFilename();
  if (_gFormat->compare("gmad")) {
    return buildGmad();
  } 
  else if (_gFormat->compare("lcdd")) {
    return buildLCDD();
  }
  else if (_gFormat->compare("mokka")) {
    return buildMokka();
  }
  else if (_gFormat->compare("gdml")) {
    return buildGDML();
  }
  else if (_gFormat->compare("none")) {
    return buildNone();
  }
  G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - error - reached end of method without building.";
  G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  return NULL;
}

void BDSGeometryFactory::parseFormatAndFilename(){
  if(_geometry != ""){
    G4int pos = _geometry.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid geometry reference format : "<<_geometry<<G4endl;
    }
    else {
      G4String format = _geometry.substr(0,pos);
      _gFormat->spec(format);
      _gFile = BDSExecOptions::Instance()->GetBDSIMPATH() + _geometry.substr(pos+1,_geometry.length() - pos); 
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



