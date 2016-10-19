#ifndef BDSFIELDLOADERPOISSON_H
#define BDSFIELDLOADERPOISSON_H

#include "G4String.hh"

class BDSArray2DCoords;

/**
 * @brief Loader for 2D Poisson SuperFish SF7 files.
 * 
 * @author Laurie Nevay
 */

class BDSFieldLoaderPoisson
{
public:
  BDSFieldLoaderPoisson();
  ~BDSFieldLoaderPoisson();

  /// Load the 2D array of 3-Vector field values.
  BDSArray2DCoords* LoadMag2D(G4String fileName);

};

#endif
