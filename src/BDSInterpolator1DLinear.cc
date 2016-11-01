#include "BDSArray1DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator1DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator1DLinear::BDSInterpolator1DLinear(BDSArray1DCoords* arrayIn):
  BDSInterpolator1D(arrayIn)
{;}

BDSInterpolator1DLinear::~BDSInterpolator1DLinear()
{;}

BDSFieldValue BDSInterpolator1DLinear::GetInterpolatedValueT(G4double x) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double x1 = floor(xarr);

  BDSFieldValue values[2];
  values[0] = array->GetConst(x1);
  values[1] = array->GetConst(x1+1);
  BDSFieldValue result = BDS::Linear1D(values, xarr-x1);
  
  return result;
}
