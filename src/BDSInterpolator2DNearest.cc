#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DNearest.hh"

BDSInterpolator2DNearest::BDSInterpolator2DNearest(BDSArray2DCoords* arrayIn):
  array(arrayIn)
{;}

BDSInterpolator2DNearest::~BDSInterpolator2DNearest()
{;}

BDSFieldValue BDSInterpolator2DNearest::GetInterpolatedValueT(G4double x,
							      G4double y) const
{return (*array)(array->NearestX(x), array->NearestY(y));}
