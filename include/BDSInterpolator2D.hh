#ifndef BDSINTERPOLATOR2D_H
#define BDSINTERPOLATOR2D_H

#include "BDSThreeVector.hh"
#include "BDSTwoVector.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

template<typename T>
class BDSInterpolator2D
{
public:
  BDSInterpolator2D();
  ~BDSInterpolator2D();

  G4ThreeVector GetInterpolatedValueV(G4double x, G4double y) const;

protected:
  virtual T GetInterpolatedValue(G4double x, G4double y) const = 0;
};

template <>
G4ThreeVector BDSInterpolator2D<G4double>::GetInterpolatedValueV(G4double x, G4double y) const
{return G4ThreeVector(GetInterpolatedValue(x,y),0,0);}

template <>
G4ThreeVector BDSInterpolator2D<G4float>::GetInterpolatedValueV(G4double x, G4double y) const
{return G4ThreeVector(GetInterpolatedValue(x,y),0,0);}

template <>
G4ThreeVector BDSInterpolator2D<G4int>::GetInterpolatedValueV(G4double x, G4double y) const
{return G4ThreeVector(GetInterpolatedValue(x,y),0,0);}

template <>
G4ThreeVector BDSInterpolator2D<G4long>::GetInterpolatedValueV(G4double x, G4double y) const
{return G4ThreeVector(GetInterpolatedValue(x,y),0,0);}

template <>
G4ThreeVector BDSInterpolator2D<BDSTwoVector<G4double> >::GetInterpolatedValueV(G4double x, G4double y) const
{
  BDSTwoVector<G4double> result = GetInterpolatedValue(x,y);
  return G4ThreeVector(result[0],result[1],0);
}

template <>
G4ThreeVector BDSInterpolator2D<BDSThreeVector<G4double> >::GetInterpolatedValueV(G4double x, G4double y) const
{
  BDSThreeVector<G4double> result = GetInterpolatedValue(x,y);
  return G4ThreeVector(result[0], result[1], result[2]);
}


#endif
