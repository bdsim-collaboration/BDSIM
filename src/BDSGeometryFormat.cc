#include "BDSGeometryFormat.hh"

BDSGeometryFormat::BDSGeometryFormat():BDSSpec((G4String)"none", allowedSpecs())
{}

BDSGeometryFormat::~BDSGeometryFormat()
{;}

BDSGeometryFormat::BDSGeometryFormat(G4String format):BDSSpec(format, allowedSpecs())
{;}

std::list<G4String> BDSGeometryFormat::allowedSpecs(){
  std::list<G4String> tmp;
  tmp.push_back((G4String)"none");
  tmp.push_back((G4String)"gmad");
  tmp.push_back((G4String)"mokka");
#ifdef USE_LCDD
  tmp.push_back((G4String)"lcdd");
#endif
#ifdef USE_GDML
  tmp.push_back((G4String)"gdml");
#endif
  return tmp;
}
