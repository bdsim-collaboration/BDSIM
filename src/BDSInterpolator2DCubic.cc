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
  G4double x2 = ceil(xarr);
  G4double y1 = floor(yarr);
  G4double y2 = ceil(yarr);

  // Central values
  BDSFieldValue Q11 = array->GetConst(x1,y1);
  BDSFieldValue Q12 = array->GetConst(x1,y2);
  BDSFieldValue Q22 = array->GetConst(x2,y2);
  BDSFieldValue Q21 = array->GetConst(x2,y1);

  // Surrounding values
  BDSFieldValue Q00 = array->GetConst(x1-1, y1-1);
  BDSFieldValue Q10 = array->GetConst(x1,   y1-1);
  BDSFieldValue Q20 = array->GetConst(x2,   y1-1);
  BDSFieldValue Q30 = array->GetConst(x2+1, y1-1);
  BDSFieldValue Q31 = array->GetConst(x2+1, y1);
  BDSFieldValue Q32 = array->GetConst(x2+1, y2);
  BDSFieldValue Q33 = array->GetConst(x2+1, y2+1);
  BDSFieldValue Q23 = array->GetConst(x2,   y2+1);
  BDSFieldValue Q13 = array->GetConst(x1,   y2+1);
  BDSFieldValue Q03 = array->GetConst(x1-1, y2+1);
  BDSFieldValue Q02 = array->GetConst(x1-1, y2);
  BDSFieldValue Q01 = array->GetConst(x1-1, y1);

  BDSFieldValue localData[4][4] =
    {
      {Q03, Q13, Q23, Q33},
      {Q02, Q12, Q22, Q32},
      {Q01, Q11, Q21, Q31},
      {Q00, Q10, Q20, Q30}
    };

  return BiCubicInterpolate(localData, xarr-x1, yarr-y1);
}
