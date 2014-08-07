#include "BDSFilename.hh"

BDSFilename::BDSFilename(){}

BDSFilename::~BDSFilename(){}

BDSFilename::BDSFilename(G4String fileName){
  _fileName = fileName;
}

G4String BDSFilename::fileName(){
  return _fileName;
}

G4String BDSFilename::fullPath(){
  G4String sBDSPATH = getEnv("BDSIMPATH");
  G4String fullPath = sBDSPATH + _fileName;
  return fullPath;
}

void BDSFilename::fileName(G4String fileName){
  _fileName = fileName;
}
