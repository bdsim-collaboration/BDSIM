#ifndef __BDS_GEOMETRY_HH_
#define __BDS_GEOMETRY_HH_

#include "BDSGeometryFormat.hh"
#include "G4LogicalVolume.hh"

class BDSGeometry{
public:
  BDSGeometry();
  ~BDSGeometry();
  BDSGeometry(BDSGeometryFormat* format, G4String file);

  BDSGeometryFormat* format() const;
  G4String file() const;

  virtual void Construct(G4LogicalVolume* marker)=0;

private:
  BDSGeometryFormat* _format;
  G4String _file;
};

#endif
