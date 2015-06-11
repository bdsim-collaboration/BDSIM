#include "BDSBmapFormat.hh"
BDSBmapFormat::BDSBmapFormat(){
  allowedSpecs();
}

BDSBmapFormat::~BDSBmapFormat()
{;}

BDSBmapFormat::BDSBmapFormat(G4String format)
{
  allowedSpecs();
  spec(format);
}


void BDSBmapFormat::allowedSpecs(){
  addAllowedSpec((G4String)"none");
  addAllowedSpec((G4String)"XY");
  addAllowedSpec((G4String)"3D");
  addAllowedSpec((G4String)"mokka");
  addAllowedSpec((G4String)"test");
}

/*
G4bool BDSBmapFormat::compare(G4String val) const{
  BDSBmapFormat* testf = new BDSBmapFormat(val);
  G4bool result = compare(testf);
  delete testf;
  return result;
}
*/
