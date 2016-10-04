#ifndef BDSARRAY3DCOORDS_H
#define BDSARRAY3DCOORDS_H

#include "BDSArray3D.hh"
#include "BDSThreeVector.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief A wrapper for a templated BDSArray3D that provides real space coordinates.
 * 
 * @author Laurie Nevay
 */

template <typename T>
class BDSArray3DCoords
{
public:
  BDSArray3DCoords(G4int nX, G4int nY, G4int nZ,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn):
    data(BDSArray3D<T>(nX,nY,nZ)),
    xMin(xMinIn), xMax(xMaxIn),
    yMin(yMinIn), yMax(yMaxIn),
    zMin(zMinIn), zMax(zMaxIn),
    defaultValue(T())
  {
    xStep = (xMax - xMin ) / (G4double)nY;
    yStep = (yMax - yMin ) / (G4double)nY;
    zStep = (zMax - zMin ) / (G4double)nZ;
  }

  inline G4double XStep() const
  {return xStep;}

  inline G4double YStep() const
  {return yStep;}

  inline G4double ZStep() const
  {return zStep;}

  inline G4bool Outside(const G4double x, const G4double y, const G4double z) const
  {
    G4bool resultX = (x < xMin) || (x > xMax);
    G4bool resultY = (y < yMin) || (y > yMax);
    G4bool resultZ = (z < zMin) || (z > zMax);
    return resultX || resultY || resultZ;
  } 

  inline G4double ArrayCoordsFromX(const G4double x) const
  {return (x - xMin) / xStep;}

  inline G4double ArrayCoordsFromY(const G4double y) const
  {return (y - yMin) / yStep;}

  inline G4double ArrayCoordsFromZ(const G4double z) const
  {return (z - zMin) / zStep;}
  
  inline BDSThreeVector<G4double> ArrayCoordsFromXYZ(const G4double x,
						     const G4double y,
						     const G4double z) const
  {return BDSThreeVector<G4double>(ArrayCoordsFromX(x), ArrayCoordsFromY(y), ArrayCoordsFromZ(z));}

  inline G4double XFromArrayCoords(const G4double xCoord) const
  {return xMin + xCoord*xStep;}

  inline G4double YFromArrayCoords(const G4double yCoord) const
  {return yMin + yCoord*yStep;}

  inline G4double ZFromArrayCoords(const G4double zCoord) const
  {return zMin + zCoord*zStep;}

  inline BDSThreeVector<G4double> XYZFromArrayCoords(const G4double x, const G4double y) const
  {return BDSThreeVector<G4double>(XFromArrayCoords(x), YFromArrayCoords(y));}
  
  inline G4int NearestX(const G4double x) const
  {return (G4int)round((x - xMin) / xStep);}

  inline G4int NearestY(const G4double y) const
  {return (G4int)round((y - yMin) / yStep);}

  inline G4int NearestZ(const G4double z) const
  {return (G4int)round((z - zMin) / zStep);}

  inline BDSThreeVector<G4int> NearestXYZ(const G4double x,
					  const G4double y,
					  const G4double z) const
  {return BDSThreeVector<G4int>(NearestX(x), NearestY(y), NearestZ(z));}

  /// Access / set a single element. Note the [] operator cannot be used as it
  /// only allows one argument. Trying to set (because non const) a value outside
  /// the array results in an error and exiting.
  T& operator()(const G4int x, const G4int y, const G4int z)
  {
    if ( (x < xMin) || (x > xMax) ||
	 (y < xMin) || (y > yMax) ||
	 (z < zMin) || (z > zMax) )
      {G4cout << "(" << x << "," << y << "," << z << ") is outside array" << G4endl; exit(1);}
    return data(x,y,z);
  }

  /// Convenience operator for two vector.
  T& operator()(const BDSThreeVector<G4int> xyz)
  {return (*this)(xyz[0], xyz[1], xyz[2]);}

  /// Access a single element in array coordinates.  Note the [] operator
  /// cannot be used as it only allows one arument. Accessing coordinates outside
  /// the array returns a default value of the teamplated parameter.
  const T& operator()(const G4int x, G4int y, G4int z) const
  {
    if ( (x < xMin) || (x > xMax) ||
	 (y < xMin) || (y > yMax) ||
	 (z < zMin) || (z > zMax) )
      {return defaultValue;}
    return data(x,y,z);
  }

  const T& operator()(const BDSThreeVector<G4int> xyz) const
  {return (*this)(xyz[0], xyz[1], xyz[2]);}
  
private:
  BDSArray3D<T> data;
  G4double xMin;
  G4double xMax;
  G4double xStep;
  G4double yMin;
  G4double yMax;
  G4double yStep;
  G4double zMin;
  G4double zMax;
  G4double zStep;

  T defaultValue;
};

#endif
