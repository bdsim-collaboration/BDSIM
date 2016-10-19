#include "BDSArray3DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator3DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator3DLinear::BDSInterpolator3DLinear(BDSArray3DCoords* arrayIn):
  BDSInterpolator3D(arrayIn)
{;}

BDSInterpolator3DLinear::~BDSInterpolator3DLinear()
{;}

BDSFieldValue BDSInterpolator3DLinear::GetInterpolatedValueT(G4double x,
							     G4double y,
							     G4double z) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromY(z);
  
  G4double x0 = floor(xarr);
  G4double x1 = ceil(xarr);
  G4double y0 = floor(yarr);
  G4double y1 = ceil(yarr);
  G4double z0 = floor(zarr);
  G4double z1 = floor(zarr);

  // The fractional part will give us the normalised distance between x0 and the point.
  // ie if x were in spatial coords, xDist = (x-x0)/(x1-x0) where x is an arbitrary point
  // between lattice points x0 and x1.
  G4double xDist = xarr - x0;
  G4double yDist = yarr - y0;
  G4double zDist = zarr - z0;

  // Get the values we requires - indices are x,y,z
  BDSFieldValue C000 = array->GetConst(x0,y0,z0);
  BDSFieldValue C001 = array->GetConst(x0,y0,z1);
  BDSFieldValue C010 = array->GetConst(x0,y1,z0);
  BDSFieldValue C011 = array->GetConst(x0,y1,z1);
  BDSFieldValue C100 = array->GetConst(x1,y0,z0);
  BDSFieldValue C101 = array->GetConst(x1,y0,z1);
  BDSFieldValue C110 = array->GetConst(x1,y1,z0);
  BDSFieldValue C111 = array->GetConst(x1,y1,z1);

  // Interpolate along x
  BDSFieldValue C00  = BDS::Linear1D(C000, C100, xDist);
  BDSFieldValue C01  = BDS::Linear1D(C001, C101, xDist);
  BDSFieldValue C10  = BDS::Linear1D(C010, C110, xDist);
  BDSFieldValue C11  = BDS::Linear1D(C011, C111, xDist);

  // Interpolate along y
  BDSFieldValue C0   = BDS::Linear1D(C00, C10, yDist);
  BDSFieldValue C1   = BDS::Linear1D(C01, C11, yDist);

  // Interpolate along z
  BDSFieldValue C    = BDS::Linear1D(C0, C1, zDist);

  return C;
}
