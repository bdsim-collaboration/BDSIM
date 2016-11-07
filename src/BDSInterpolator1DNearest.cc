#include "BDSArray1DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator1DNearest.hh"


BDSInterpolator1DNearest::BDSInterpolator1DNearest(BDSArray1DCoords* arrayIn):
  BDSInterpolator1D(arrayIn)
{;}

BDSInterpolator1DNearest::~BDSInterpolator1DNearest()
{;}

BDSFieldValue BDSInterpolator1DNearest::GetInterpolatedValueT(G4double x) const
{
  G4int xind = array->NearestX(x);
  BDSFieldValue result = array->GetConst(xind); // here we're constructing a copy on purpose
  return result;
}
