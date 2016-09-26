#ifndef BDSFIELDLOADERPOISSON_H
#define BDSFIELDLOADERPOISSON_H

#include "G4String.hh"

class BDSArray2DCoords;

class BDSFieldLoaderPoisson
{
public:
  BDSFieldLoaderPoisson();
  ~BDSFieldLoaderPoisson();

  BDSArray2DCoords* LoadMag2D(G4String fileName);

};

#endif
