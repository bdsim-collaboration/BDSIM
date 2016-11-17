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
  G4double y0 = floor(yarr);
  G4double z0 = floor(zarr);
  G4double t0 = floor(tarr);

  BDSFieldValue localData[2][2][2][2];

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{
	  for (int k = 0; k < 2; k++)
	    {
	      for (int l = 0; l < 2; l++)
		{localData[i][j][k][l] = array->GetConst(x0+i, y0+j, z0+k, t0+l);}
	    }
	}
    }

  BDSFieldValue result = BDS::Linear4D(localData, xarr-x0, yarr-y0, zarr-z0, tarr-t0);

  return result;
}
