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
  G4double zarr = array->ArrayCoordsFromZ(z);
  
  G4double x0 = floor(xarr);
  G4double y0 = floor(yarr);
  G4double z0 = floor(zarr);

  BDSFieldValue localData[2][2][2];

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{
	  for (int k = 0; k < 2; k++)
	    {localData[i][j][k] = array->GetConst(x0+i, y0+j, z0+k);}
	}
    }

  BDSFieldValue result = BDS::Linear3D(localData, xarr-x0, yarr-y0, zarr-z0);

  return result;
}
