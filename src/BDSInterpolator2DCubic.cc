#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DCubic.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator2DCubic::BDSInterpolator2DCubic(BDSArray2DCoords* arrayIn):
  BDSInterpolator2D(arrayIn)
{;}

BDSInterpolator2DCubic::~BDSInterpolator2DCubic()
{;}

BDSFieldValue BDSInterpolator2DCubic::CubicInterpolate(const BDSFieldValue p[4],
						       const G4double      x) const
{
  return p[1]+0.5 * x*(p[2]-p[0]+x*(2.*p[0]-5.*p[1]+4.*p[2]-p[3]+x*(3.*(p[1]-p[2])+p[3]-p[0])));
}

BDSFieldValue BDSInterpolator2DCubic::BiCubicInterpolate(const BDSFieldValue p[4][4],
							 const G4double      x,
							 const G4double      y) const
{
  BDSFieldValue arr[4];
  arr[0] = CubicInterpolate(p[0], y);
  arr[1] = CubicInterpolate(p[1], y);
  arr[2] = CubicInterpolate(p[2], y);
  arr[3] = CubicInterpolate(p[3], y);
  return CubicInterpolate(arr, x);
}

BDSFieldValue BDSInterpolator2DCubic::TriCubicInterpolate(const BDSFieldValue p[4][4][4],
							  const G4double      x,
							  const G4double      y,
							  const G4double      z) const
{
  BDSFieldValue arr[4];
  arr[0] = BiCubicInterpolate(p[0], y, z);
  arr[1] = BiCubicInterpolate(p[1], y, z);
  arr[2] = BiCubicInterpolate(p[2], y, z);
  arr[3] = BiCubicInterpolate(p[3], y, z);
  return CubicInterpolate(arr, x);
}

BDSFieldValue BDSInterpolator2DCubic::GetInterpolatedValueT(G4double x, G4double y) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);

  // Array indicies will look like this where point lies in unit (in array coords)
  // square between 11,12,22,21.

  // 03 13 23 33
  // 02 12 22 32
  // 01 11 21 31
  // 00 10 20 30
  
  G4double x1 = floor(xarr);
  G4double y1 = floor(yarr);

  BDSFieldValue localData[4][4];

  G4double x0 = x1-1;
  G4double y0 = y1-1;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{localData[i][j] = array->GetConst(x0+i, y0+j);}
    }

  return BiCubicInterpolate(localData, xarr-x1, yarr-y1);
}
