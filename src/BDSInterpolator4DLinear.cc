#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator4DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator4DLinear::BDSInterpolator4DLinear(BDSArray4DCoords* arrayIn):
  BDSInterpolator4D(arrayIn)
{;}

BDSInterpolator4DLinear::~BDSInterpolator4DLinear()
{;}

BDSFieldValue BDSInterpolator4DLinear::Interpolate1D(const BDSFieldValue& v1,
						     const BDSFieldValue& v2,
						     const G4double       dist) const
{
  return v1*(1.-dist) + v2*dist;
}

BDSFieldValue BDSInterpolator4DLinear::GetInterpolatedValueT(G4double x,
							     G4double y,
							     G4double z,
							     G4double t) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromY(z);
  G4double tarr = array->ArrayCoordsFromT(t);
  
  G4double x0 = floor(xarr);
  //G4double x1 = ceil(xarr);
  G4double y0 = floor(yarr);
  //G4double y1 = ceil(yarr);
  G4double z0 = floor(zarr);
  //G4double z1 = ceil(zarr);
  G4double t0 = floor(tarr);
  //G4double t1 = ceil(tarr);

  BDSFieldValue localData[2][2][2][2];

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{
	  for (int k = 0; k < 2; k++)
	    {
	      for (int l = 0; l < 2; l++)
		{localData[i][j][k][l] = array->GetConst(x0+i, y0+i, z0+i);}
	    }
	}
    }

  BDSFieldValue result = BDS::Linear4D(localData, xarr-x0, yarr-y0, zarr-z0, tarr-t0);

  return result;
  /*

  // The fractional part will give us the normalised distance between x0 and the point.
  // ie if x were in spatial coords, xDist = (x-x0)/(x1-x0) where x is an arbitrary point
  // between lattice points x0 and x1.
  G4double xDist = xarr - x0;
  G4double yDist = yarr - y0;
  G4double zDist = zarr - z0;
  G4double tDist = tarr - t0;

  // Get the values we requires - indices are x,y,z
  BDSFieldValue C0000 = array->GetConst(x0,y0,z0,t0);
  BDSFieldValue C0010 = array->GetConst(x0,y0,z1,t0);
  BDSFieldValue C0100 = array->GetConst(x0,y1,z0,t0);
  BDSFieldValue C0110 = array->GetConst(x0,y1,z1,t0);
  BDSFieldValue C1000 = array->GetConst(x1,y0,z0,t0);
  BDSFieldValue C1010 = array->GetConst(x1,y0,z1,t0);
  BDSFieldValue C1100 = array->GetConst(x1,y1,z0,t0);
  BDSFieldValue C1110 = array->GetConst(x1,y1,z1,t0);
  BDSFieldValue C0001 = array->GetConst(x0,y0,z0,t1);
  BDSFieldValue C0011 = array->GetConst(x0,y0,z1,t1);
  BDSFieldValue C0101 = array->GetConst(x0,y1,z0,t1);
  BDSFieldValue C0111 = array->GetConst(x0,y1,z1,t1);
  BDSFieldValue C1001 = array->GetConst(x1,y0,z0,t1);
  BDSFieldValue C1011 = array->GetConst(x1,y0,z1,t1);
  BDSFieldValue C1101 = array->GetConst(x1,y1,z0,t1);
  BDSFieldValue C1111 = array->GetConst(x1,y1,z1,t1);

  // For t0
  // Interpolate along x
  BDSFieldValue C00_0 = Interpolate1D(C0000, C1000, xDist);
  BDSFieldValue C01_0 = Interpolate1D(C0010, C1010, xDist);
  BDSFieldValue C10_0 = Interpolate1D(C0100, C1100, xDist);
  BDSFieldValue C11_0 = Interpolate1D(C0110, C1110, xDist);

  // Interpolate along y
  BDSFieldValue C0__0 = Interpolate1D(C00_0, C10_0, yDist);
  BDSFieldValue C1__0 = Interpolate1D(C01_0, C11_0, yDist);

  // Interpolate along z
  BDSFieldValue C___0 = Interpolate1D(C0__0, C1__0, zDist);

  // For t1
  // Interpolate along x
  BDSFieldValue C00_1 = Interpolate1D(C0001, C1001, xDist);
  BDSFieldValue C01_1 = Interpolate1D(C0011, C1011, xDist);
  BDSFieldValue C10_1 = Interpolate1D(C0101, C1101, xDist);
  BDSFieldValue C11_1 = Interpolate1D(C0111, C1111, xDist);

  // Interpolate along y
  BDSFieldValue C0__1 = Interpolate1D(C00_1, C10_1, yDist);
  BDSFieldValue C1__1 = Interpolate1D(C01_1, C11_1, yDist);

  // Interpolate along z
  BDSFieldValue C___1 = Interpolate1D(C0__1, C1__1, zDist);

  // Interpolate in t
  BDSFieldValue C = Interpolate1D(C___0, C___1, tDist);
  
  return C;
  */
}
