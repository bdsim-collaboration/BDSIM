#include "BDSFieldData.hh"
#include "BDSInterpolator2DLinear.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator2DLinear::BDSInterpolator2DLinear(BDSArray2DCoords3VF* arrayIn):
  array(arrayIn)
{;}

BDSInterpolator2DLinear::~BDSInterpolator2DLinear()
{;}

BDSThreeVectorF BDSInterpolator2DLinear::GetInterpolatedValueT(G4double x, G4double y) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  
  G4double x1 = floor(xarr);
  G4double x2 = ceil(xarr);
  G4double y1 = floor(yarr);
  G4double y2 = ceil(yarr);
  
  BDSThreeVectorF Q11 = (*array)(x1,y1);
  BDSThreeVectorF Q12 = (*array)(x1,y2);
  BDSThreeVectorF Q22 = (*array)(x2,y2);
  BDSThreeVectorF Q21 = (*array)(x2,y1);
  
  G4double x2mx = x2 - x;
  G4double y2my = y2 - y;
  G4double xmx1 = x  - x1;
  G4double ymy1 = y  - y1;
  G4double factor = 1./(array->XStep()*array->YStep());
  BDSThreeVectorF result = (Q11 * x2mx * y2my) +
    (Q21 * xmx1 * y2my) +
    (Q12 * x2mx * ymy1) +
    (Q22 * xmx1 * ymy1);
  
  result *= factor;
  return result;
  }
