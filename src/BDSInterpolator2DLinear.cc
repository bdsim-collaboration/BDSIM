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
  //G4double x2 = ceil(xarr);
  G4double y1 = floor(yarr);
  //G4double y2 = ceil(yarr);

  BDSFieldValue localData[2][2];
  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{localData[i][j] = array->GetConst(x1+i, y1+i);}
    }

  /*
  BDSFieldValue Q11 = array->GetConst(x1,y1);
  BDSFieldValue Q12 = array->GetConst(x1,y2);
  BDSFieldValue Q22 = array->GetConst(x2,y2);
  BDSFieldValue Q21 = array->GetConst(x2,y1);
  
  G4double x2mx = x2 - xarr;
  G4double y2my = y2 - yarr;
  G4double xmx1 = xarr - x1;
  G4double ymy1 = yarr - y1;
  BDSFieldValue result = (Q11 * x2mx * y2my) +
    (Q21 * xmx1 * y2my) +
    (Q12 * x2mx * ymy1) +
    (Q22 * xmx1 * ymy1);
  */

  BDSFieldValue result = BDS::Linear2D(localData, xarr-x1, yarr-y1);
  
  return result;
}
