#ifndef __BDS_3D_MAG_FIELD_HH_
#define __BDS_3D_MAG_FIELD_HH_
//Based on the Geant4 example examples/advanced/purging_magnet/src/PurgMagTabulatedField3D.cc
//#include "G4MagneticField.hh"
#include "G4ios.hh"
#include "BDSMagField.hh"

#include <fstream>
#include <vector>
#include <cmath>

class BDS3DMagField : public BDSMagField
{
  // Storage space for the table
  std::vector< std::vector< std::vector< double > > > xField;
  std::vector< std::vector< std::vector< double > > > yField;
  std::vector< std::vector< std::vector< double > > > zField;
  // The dimensions of the table
  int nx,ny,nz; 
  // The physical limits of the defined region
  double minx, maxx, miny, maxy, minz, maxz;
  // The physical extent of the defined region
  double dx, dy, dz;
  double fZoffset, fXoffset, fYoffset;
  bool invertX, invertY, invertZ;
  double _lenUnit, _fieldUnit;
  
public:
  BDS3DMagField(const char* filename, double zOffset, double xOffset );
  void  GetFieldValue( const  double Point[4],
		       double *Bfield          ) const;
  virtual void Prepare(G4VPhysicalVolume *referenceVolume);
};

#endif
