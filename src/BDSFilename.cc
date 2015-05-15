#include "BDSFilename.hh"

#include "BDSExecOptions.hh"

BDSFilename::BDSFilename(){}

BDSFilename::~BDSFilename(){}

BDSFilename::BDSFilename(G4String fileName){
  _fileName = fileName;
}

G4String BDSFilename::fileName(){
  return _fileName;
}

G4String BDSFilename::fullPath(){
  G4String sBDSPATH = BDSExecOptions::Instance()->GetBDSIMPATH();
  G4String fullPath = sBDSPATH + _fileName;
  return fullPath;
}

void BDSFilename::fileName(G4String fileName){
  _fileName = fileName;
}
