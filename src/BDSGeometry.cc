#include "BDSGeometry.hh"

BDSGeometry::BDSGeometry(){
  init();
}

BDSGeometry::~BDSGeometry()
{;}


/*
BDSGeometry::BDSGeometry(const char* format, G4String file){
  BDSGeometry((G4String)format,file);
}

BDSGeometry::BDSGeometry(G4String format, G4String file){
  BDSGeometry(new BDSGeometryFormat(format), file);
}
*/

BDSGeometry::BDSGeometry(BDSGeometryFormat* format, G4String file):_format(format),_file(file)
{
  init();
}


void BDSGeometry::init(){
  _align_in_volume=NULL;
  _align_out_volume=NULL;
  _format=NULL;
  _file="";
  _length=0;
  _field=NULL;
}


BDSGeometryFormat* BDSGeometry::format() const{
  return _format;
}

G4String BDSGeometry::file() const{
  return _file;
}
