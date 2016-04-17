#ifndef BDSINTERPOLATOR2D_H
#define BDSINTERPOLATOR2D_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

template<typename T>
class BDSInterpolator2D
{
public:
  BDSInterpolator2D();
  ~BDSInterpolator2D();

  G4ThreeVector GetInterpolatedValueV(G4double x, G4double y) const;

  virtual T GetInterpolatedValue(G4double x, G4double y) const = 0;
};

#endif
