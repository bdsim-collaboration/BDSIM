#include "BDSGeometryFactory.hh"
#include "BDSExecOptions.hh"
#include "ggmad.hh"
#ifdef USE_LCDD
#include "BDSGeometryLCDD.hh"
#endif
#include "BDSGeometrySQL.hh"
#ifdef USE_GDML
#include "BDSGeometryGDML.hh"
#endif
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
#ifdef USE_LCDD
  else if (_gFormat->compare("lcdd")) {
    return buildLCDD();
  }
#endif
  else if (_gFormat->compare("mokka")) {
    return buildMokka();
  }
#ifdef USE_GDML
  else if (_gFormat->compare("gdml")) {
    return buildGDML();
  }
#endif
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

#ifdef USE_LCDD
BDSGeometry* BDSGeometryFactory::buildLCDD(){
  return new BDSGeometryLCDD(_gFile);
}
#endif

BDSGeometry* BDSGeometryFactory::buildMokka(){
  return new BDSGeometrySQL(_gFile);
}

#ifdef USE_GDML
BDSGeometry* BDSGeometryFactory::buildGDML(){
  return new BDSGeometryGDML(_gFile);
}
#endif

BDSGeometry* BDSGeometryFactory::buildNone(){
  return NULL;
}



