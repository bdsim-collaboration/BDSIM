#include "BDSBmapFormat.hh"
BDSBmapFormat::BDSBmapFormat():BDSSpec(G4String("none"), allowedSpecs()){
}

BDSBmapFormat::~BDSBmapFormat()
{;}

BDSBmapFormat::BDSBmapFormat(G4String format):BDSSpec(format, allowedSpecs())
{;}


std::list<G4String> BDSBmapFormat::allowedSpecs(){
  std::list<G4String> tmp;
  tmp.push_back((G4String)"none");
  tmp.push_back((G4String)"XY");
  tmp.push_back((G4String)"3D");
  tmp.push_back((G4String)"mokka");
  tmp.push_back((G4String)"test");
  return tmp;
}
