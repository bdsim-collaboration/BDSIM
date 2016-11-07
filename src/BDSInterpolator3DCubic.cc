#include "BDSArray3DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator3DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator3DCubic::BDSInterpolator3DCubic(BDSArray3DCoords* arrayIn):
  BDSInterpolator3D(arrayIn)
{;}

BDSInterpolator3DCubic::~BDSInterpolator3DCubic()
{;}

BDSFieldValue BDSInterpolator3DCubic::GetInterpolatedValueT(G4double x,
							    G4double y,
							    G4double z) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromZ(z);
  
  G4double x1 = floor(xarr);
  G4double y1 = floor(yarr);
  G4double z1 = floor(zarr);

  BDSFieldValue localData[4][4][4];

  G4double x0 = x1-1;
  G4double y0 = y1-1;
  G4double z0 = z1-1;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  for (int k = 0; k < 4; k++)
	    {localData[i][j][k] = array->GetConst(x0+i, y0+j, z0+k);}
	}
    }

  return BDS::Cubic3D(localData, xarr-x1, yarr-y1, zarr-z1);
}
