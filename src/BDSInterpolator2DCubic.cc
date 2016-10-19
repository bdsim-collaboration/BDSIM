#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator2DCubic::BDSInterpolator2DCubic(BDSArray2DCoords* arrayIn):
  BDSInterpolator2D(arrayIn)
{;}

BDSInterpolator2DCubic::~BDSInterpolator2DCubic()
{;}

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

  return BDS::Cubic2D(localData, xarr-x1, yarr-y1);
}
