#include "BDSArray2DCoords.hh"
#include "BDSFieldLoaderPoisson.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"

BDSFieldLoaderPoisson::BDSFieldLoaderPoisson()
{;}

BDSFieldLoaderPoisson::~BDSFieldLoaderPoisson()
{;}

BDSArray2DCoords* BDSFieldLoaderPoisson::LoadMag2D(G4String /*file*/)
{
  return new BDSArray2DCoords(10,20,0,10,0,10);
}
