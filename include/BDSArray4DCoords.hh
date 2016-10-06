#ifndef BDSARRAY4DCOORDS_H
#define BDSARRAY4DCOORDS_H

#include "BDSArray4D.hh"
#include "BDSFieldValue.hh"
#include "BDSFourVector.hh"

#include "globals.hh"

#include <ostream>

class BDSArray4DCoords: public BDSArray4D
{
public:
  BDSArray4DCoords(G4int nX, G4int nY, G4int nZ, G4int nT,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn,
		   G4double tMinIn, G4double tMaxIn);

  virtual ~BDSArray4DCoords(){;} 
  
  inline G4double XStep() const {return xStep;}
  inline G4double YStep() const {return yStep;}
  inline G4double ZStep() const {return zStep;}
  inline G4double TStep() const {return tStep;}

  virtual G4bool OutsideCoords(const G4double x,
			       const G4double y,
			       const G4double z,
			       const G4double t) const;

  void OutsideCoordsWarn(const G4double x,
			 const G4double y,
			 const G4double z,
			 const G4double t) const;

  /// @{ Not much point in being both virtual and inline (in our use case) but has to be virtual.
  virtual G4double ArrayCoordsFromX(const G4double x) const {return (x - xMin) / xStep;}
  virtual G4double ArrayCoordsFromY(const G4double y) const {return (y - yMin) / yStep;}
  virtual G4double ArrayCoordsFromZ(const G4double z) const {return (z - zMin) / zStep;}
  virtual G4double ArrayCoordsFromT(const G4double t) const {return (t - tMin) / tStep;}
  /// @}

  inline BDSFourVector<G4double> ArrayCoordsFromXYZT(const G4double x,
						     const G4double y,
						     const G4double z,
						     const G4double t) const
  {
    return BDSFourVector<G4double>(ArrayCoordsFromX(x),
				   ArrayCoordsFromY(y),
				   ArrayCoordsFromZ(z),
				   ArrayCoordsFromT(t));
  }

  inline G4double XFromArrayCoords(const G4double xCoord) const {return xMin + xCoord*xStep;}
  inline G4double YFromArrayCoords(const G4double yCoord) const {return yMin + yCoord*yStep;}
  inline G4double ZFromArrayCoords(const G4double zCoord) const {return zMin + zCoord*zStep;}
  inline G4double TFromArrayCoords(const G4double tCoord) const {return tMin + tCoord*tStep;}

  inline BDSFourVector<G4double> XYZFromArrayCoords(const G4double x,
						    const G4double y,
						    const G4double z,
						    const G4double t) const
  {
    return BDSFourVector<G4double>(XFromArrayCoords(x),
				   YFromArrayCoords(y),
				   ZFromArrayCoords(z),
				   TFromArrayCoords(t));
  }

  /// @{ Not much point in being both virtual and inline (in our use case) but has to be virtual.
  virtual G4int NearestX(const G4double x) const {return (G4int)round((x - xMin) / xStep);}
  virtual G4int NearestY(const G4double y) const {return (G4int)round((y - yMin) / yStep);}
  virtual G4int NearestZ(const G4double z) const {return (G4int)round((z - zMin) / zStep);}
  virtual G4int NearestT(const G4double t) const {return (G4int)round((t - tMin) / tStep);}
  /// @}
  
  inline BDSFourVector<G4int> NearestXYZT(const G4double x,
					  const G4double y,
					  const G4double z,
					  const G4double t) const
  {
    return BDSFourVector<G4int>(NearestX(x),
				NearestY(y),
				NearestZ(z),
				NearestT(t));
  }
  inline BDSFourVector<G4int> NearestXYZT(const BDSFourVector<G4double> pos) const
  {
    return BDSFourVector<G4int>(NearestX(pos[0]),
				NearestY(pos[1]),
				NearestZ(pos[2]),
				NearestT(pos[3]));
  }

  virtual std::ostream& Print(std::ostream& out) const;
  
  friend std::ostream& operator<< (std::ostream& out, BDSArray4DCoords const &a);

protected:
  G4double xMin;
  G4double xMax;
  G4double yMin;
  G4double yMax;
  G4double zMin;
  G4double zMax;
  G4double tMin;
  G4double tMax;

  G4double xStep;
  G4double yStep;
  G4double zStep;
  G4double tStep;
  
private:
  BDSArray4DCoords() = delete;
};

#endif
