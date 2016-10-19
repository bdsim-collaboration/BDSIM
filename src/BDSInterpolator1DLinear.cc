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

  BDSFieldValue Q1 = array->GetConst(x1);
  BDSFieldValue Q2 = array->GetConst(x1+1);
  BDSFieldValue result = BDS::Linear1D(Q1, Q2, xarr-x1);
  
  return result;
}
