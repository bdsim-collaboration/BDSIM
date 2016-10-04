#ifndef BDSINTERPOLATOR3D_H
#define BDSINTERPOLATOR3D_H

#include "BDSThreeVector.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

template<typename T>
class BDSInterpolator3D
{
public:
  BDSInterpolator3D();
  ~BDSInterpolator3D();

  G4ThreeVector GetInterpolatedValueV(G4double x, G4double y, G4double z) const;

protected:
  virtual T GetInterpolatedValue(G4double x, G4double y, G4double z) const = 0;
};

template <>
G4ThreeVector BDSInterpolator3D<G4double>::GetInterpolatedValueV(G4double x, G4double y, G4double z) const
{return G4ThreeVector(GetInterpolatedValue(x,y,z),0,0);}

template <>
G4ThreeVector BDSInterpolator3D<G4float>::GetInterpolatedValueV(G4double x, G4double y, G4double z) const
{return G4ThreeVector(GetInterpolatedValue(x,y,z),0,0);}

template <>
G4ThreeVector BDSInterpolator3D<G4int>::GetInterpolatedValueV(G4double x, G4double y, G4double z) const
{return G4ThreeVector(GetInterpolatedValue(x,y,z),0,0);}

template <>
G4ThreeVector BDSInterpolator3D<G4long>::GetInterpolatedValueV(G4double x, G4double y, G4double z) const
{return G4ThreeVector(GetInterpolatedValue(x,y,z),0,0);}

template <>
G4ThreeVector BDSInterpolator3D<BDSThreeVector<G4float> >::GetInterpolatedValueV(G4double x, G4double y, G4double z) const
{
  BDSThreeVector<G4float> result = GetInterpolatedValue(x,y,z);
  return G4ThreeVector(result[0], result[1], result[2]);
}

template <>
G4ThreeVector BDSInterpolator3D<BDSThreeVector<G4double> >::GetInterpolatedValueV(G4double x, G4double y, G4double z) const
{
  BDSThreeVector<G4double> result = GetInterpolatedValue(x,y,z);
  return G4ThreeVector(result[0], result[1], result[2]);
}


#endif
