#ifndef BDSMAGFIELD3D_H
#define BDSMAGFIELD3D_H

#include "BDSMagFieldMesh.hh"

#include "globals.hh"       // geant4 types / globals
#include "G4ThreeVector.hh"

#include <vector>

/**
 * @brief
 *
 * Based on the Geant4 example examples/advanced/purging_magnet/src/PurgMagTabulatedField3D.cc
 *
 */

class BDSMagField3D: public BDSMagFieldMesh
{
public:
  BDSMagField3D(const char* filename, G4ThreeVector offset);
  void  GetFieldValue( const  double Point[4],
		       double *Bfield          ) const;

  /// Convert the local coordinates of the provided field mesh into global coordinates.
  virtual void Prepare(G4VPhysicalVolume* referenceVolume);

private:
  ///@{ Storage space for the table
  std::vector< std::vector< std::vector< double > > > xField;
  std::vector< std::vector< std::vector< double > > > yField;
  std::vector< std::vector< std::vector< double > > > zField;
  ///@}
  
  /// The dimensions of the table
  int nx,ny,nz; 

  /// The physical limits of the defined region
  double minx, maxx, miny, maxy, minz, maxz;

  /// The physical extent of the defined region
  double dx, dy, dz;

  /// Offset of b field coordinate system w.r.t. geometry centre.
  G4ThreeVector offset;
  //double fZoffset, fXoffset, fYoffset;

  bool invertX, invertY, invertZ;

  double lenUnit, fieldUnit;
};

#endif
