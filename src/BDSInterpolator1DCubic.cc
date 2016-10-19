#include "BDSArray1DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator1DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator1DCubic::BDSInterpolator1DCubic(BDSArray1DCoords* arrayIn):
  BDSInterpolator1D(arrayIn)
{;}

BDSInterpolator1DCubic::~BDSInterpolator1DCubic()
{;}

BDSFieldValue BDSInterpolator1DCubic::GetInterpolatedValueT(G4double x) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double x1   = floor(xarr);

  BDSFieldValue localData[4];

  G4double x0 = x1-1;
  for (int i = 0; i < 4; i++)
    {localData[i] = array->GetConst(x0+i);}

  return BDS::Cubic1D(localData, xarr-x1);
}
