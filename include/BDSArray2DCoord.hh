#ifndef BDSARRAY2DCOORDS_H
#define BDSARRAY2DCOORDS_H

#include "BDSArray2D.hh"

#include "globals.hh"

template <typename T>
class BDSArray2DCoords
{
public:
  BDSArray2DCoords(G4int nX, G4int nY,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn):
    data(BDSArray2D<T>(nX,nY)),
    xMin(xMinIn), xMax(xMaxIn),
    yMin(yMinIn), yMax(yMaxIn),
    defaultValue(T())
  {
    xStep = (xMax - xMin ) / (G4double)nY;
    yStep = (yMax - yMin ) / (G4double)nY;
  }

  G4int NearestX(const G4double x) const
  {return (G4int)round((x - xMin) / xStep);}

  G4int NearestY(const G4double y) const
  {return (G4int)round((y - yMin) / yStep);}   

  T& operator()(const G4int x, const G4int y)
  {
    if ( (x < xMin) || (x > xMax) || (y < xMin) || (y > yMax) )
      {G4cout << "(" << x << "," << y << ") is outside array" << G4endl; exit(1);}
    return data(x,y);
  }
  
  const T& operator()(const G4double x, G4double y) const
  {
    if ( (x < xMin) || (x > xMax) || (y < xMin) || (y > yMax) )
      {return defaultValue;}
    return data(x,y);
  }
  
private:

  BDSArray2D<T> data;
  G4double xMin;
  G4double xMax;
  G4double xStep;
  G4double yMin;
  G4double yMax;
  G4double yStep;

  T defaultValue;
};

#endif
