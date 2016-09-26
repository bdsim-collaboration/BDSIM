#ifndef BDSARRAY4D_H
#define BDSARRAY4D_H

#include "BDSFieldValue.hh"
#include "BDSFourVector.hh"

#include <ostream>
#include <vector>

class BDSArray4D
{
public:
  BDSArray4D(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIN);
  virtual ~BDSArray4D(){;}

  inline G4int sizeX() const {return nX;}
  inline G4int sizeY() const {return nY;}
  inline G4int sizeZ() const {return nZ;}
  inline G4int sizeT() const {return nT;}

  /// Setter & (technically, a non-const) accessor.
  virtual BDSFieldValue& operator()(const G4int x,
				    const G4int y = 0,
				    const G4int z = 0,
				    const G4int t = 0);
  BDSFieldValue& operator()(const BDSFourVector<G4int> pos)
  {return operator()(pos.x(), pos.y(), pos.z(), pos.t());}

  /// Accessor only.
  virtual const BDSFieldValue& operator()(const G4int x,
					  const G4int y = 0,
					  const G4int z = 0,
					  const G4int t = 0) const;
  const BDSFieldValue& operator()(const BDSFourVector<G4int> pos) const
  {return operator()(pos.x(), pos.y(), pos.z(), pos.t());}

  virtual G4bool Outside(const G4int x,
			 const G4int y,
			 const G4int z,
			 const G4int t) const;

  virtual void OutsideWarn(const G4int x,
			   const G4int y,
			   const G4int z,
			   const G4int t) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray4D const &a);
  
private:
  BDSArray4D() = delete;

  const G4int nX;
  const G4int nY;
  const G4int nZ;
  const G4int nT;

  std::vector<BDSFieldValue> data;
};

#endif
