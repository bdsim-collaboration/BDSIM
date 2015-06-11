#include "BDSGeometryFormat.hh"
#include "BDSDebug.hh"

BDSGeometryFormat::BDSGeometryFormat()
{ 
  allowedSpecs();
}

BDSGeometryFormat::~BDSGeometryFormat()
{;}

BDSGeometryFormat::BDSGeometryFormat(G4String format)
{
  allowedSpecs();
  spec(format);
}

void BDSGeometryFormat::allowedSpecs(){
  addAllowedSpec((G4String)"none");
  addAllowedSpec((G4String)"gmad");
  addAllowedSpec((G4String)"mokka");
#ifdef USE_LCDD
  addAllowedSpec((G4String)"lcdd");
#endif
#ifdef USE_GDML
  addAllowedSpec((G4String)"gdml");
#endif
}

/*
G4bool BDSGeometryFormat::compare(G4String val) const{
  BDSGeometryFormat* testf = new BDSGeometryFormat(val);
  G4bool result = compare(testf);
  delete testf;
  return result;
}
*/
