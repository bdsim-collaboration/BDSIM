#include "BDSGeometry.hh"

BDSGeometry::BDSGeometry(){
  init();
}

BDSGeometry::~BDSGeometry()
{;}

BDSGeometry::BDSGeometry(BDSGeometryType format, G4String file):
  _format(format),
  _file(file)
{
  _align_in_volume = nullptr;
  _align_out_volume = nullptr;
  _length=0;
  _field = nullptr;
}

G4String BDSGeometry::file() const{
  return _file;
}
