#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator2DLinear::BDSInterpolator2DLinear(BDSArray2DCoords* arrayIn):
  BDSInterpolator2D(arrayIn)
{;}

BDSInterpolator2DLinear::~BDSInterpolator2DLinear()
{;}

BDSFieldValue BDSInterpolator2DLinear::GetInterpolatedValueT(G4double x, G4double y) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  
  G4double x1 = floor(xarr);
  G4double y1 = floor(yarr);

  BDSFieldValue localData[2][2];
  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{localData[i][j] = array->GetConst(x1+i, y1+j);}
    }
  
  BDSFieldValue result = BDS::Linear2D(localData, xarr-x1, yarr-y1);
  
  return result;
}
