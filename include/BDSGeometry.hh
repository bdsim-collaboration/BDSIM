#ifndef __BDS_GEOMETRY_HH_
#define __BDS_GEOMETRY_HH_

#include "BDSGeometryFormat.hh"

class BDSGeometry{
public:
  BDSGeometry();
  ~BDSGeometry();
  BDSGeometry(BDSGeometryFormat* format, G4String file);

  BDSGeometryFormat* format() const;
  G4String file() const;

private:
  BDSGeometryFormat* _format;
  G4String _file;
};

#endif
