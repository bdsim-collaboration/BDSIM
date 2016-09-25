#include "BDSFieldData.hh"
#include "BDSInterpolator2DNearest.hh"

#include "globals.hh" // geant4 types / gloals

BDSInterpolator2DNearest::BDSInterpolator2DNearest(BDSArray2DCoords3VF* arrayIn):
  array(arrayIn)
{;}

BDSInterpolator2DNearest::~BDSInterpolator2DNearest()
{;}

BDSThreeVectorF BDSInterpolator2DNearest::GetInterpolatedValueT(G4double x,
								G4double y) const
{return (*array)(array->NearestXY(x,y));}
