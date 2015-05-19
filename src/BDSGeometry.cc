#include "BDSGeometry.hh"

BDSGeometry::BDSGeometry()
{;}

BDSGeometry::~BDSGeometry()
{;}


BDSGeometry::BDSGeometry(BDSGeometryFormat* format, G4String file):_format(format),_file(file)
{;}


BDSGeometryFormat* BDSGeometry::format() const{
  return _format;
}

G4String BDSGeometry::file() const{
  return _file;
}
