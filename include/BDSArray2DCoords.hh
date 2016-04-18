#ifndef BDSARRAY2DCOORDS_H
#define BDSARRAY2DCOORDS_H

#include "BDSArray2D.hh"
#include "BDSTwoVector.hh"

#include "globals.hh" // geant4 types / globals

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

  inline G4double XStep() const
  {return xStep;}

  inline G4double YStep() const
  {return yStep;}

  inline G4bool Outside(const G4double x, const G4double y) const
  {
    G4bool resultX = (x < xMin) || (x > xMax);
    G4bool resultY = (y < yMin) || (y > yMax);
    return resultX || resultY;
  } 

  inline G4double ArrayCoordsFromX(const G4double x) const
  {return (x - xMin) / xStep;}

  inline G4double ArrayCoordsFromY(const G4double y) const
  {return (y - yMin) / yStep;}
  
  inline BDSTwoVector<G4double> ArrayCoordsFromXY(const G4double x, const G4double y) const
  {return BDSTwoVector<G4double>(ArrayCoordsFromX(x), ArrayCoordsFromY(y));}

  inline G4double XFromArrayCoords(const G4double xCoord) const
  {return xMin + xCoord*xStep;}

  inline G4double YFromArrayCoords(const G4double yCoord) const
  {return yMin + yCoord*yStep;}

  inline BDSTwoVector<G4double> XYFromArrayCoords(const G4double x, const G4double y) const
  {return BDSTwoVector<G4double>(XFromArrayCoords(x), YFromArrayCoords(y));}
  
  inline G4int NearestX(const G4double x) const
  {return (G4int)round((x - xMin) / xStep);}

  inline G4int NearestY(const G4double y) const
  {return (G4int)round((y - yMin) / yStep);}

  inline BDSTwoVector<G4int> NearestXY(const G4double x, const G4double y) const
  {return BDSTwoVector<G4int>(NearestX(x), NearestY(y));}

  /// Access / set a single element. Note the [] operator cannot be used as it
  /// only allows one argument. Trying to set (because non const) a value outside
  /// the array results in an error and exiting.
  T& operator()(const G4int x, const G4int y)
  {
    if ( (x < xMin) || (x > xMax) || (y < xMin) || (y > yMax) )
      {G4cout << "(" << x << "," << y << ") is outside array" << G4endl; exit(1);}
    return data(x,y);
  }

  /// Convenience operator for two vector.
  T& operator()(const BDSTwoVector<G4int> xy)
  {return (*this)(xy[0], xy[1]);}

  /// Access a single element in array coordinates.  Note the [] operator
  /// cannot be used as it only allows one arument. Accessing coordinates outside
  /// the array returns a default value of the teamplated parameter.
  const T& operator()(const G4int x, G4int y) const
  {
    if ( (x < xMin) || (x > xMax) || (y < xMin) || (y > yMax) )
      {return defaultValue;}
    return data(x,y);
  }

  const T& operator()(const BDSTwoVector<G4int> xy) const
  {return (*this)(xy[0], xy[1]);}
  
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
