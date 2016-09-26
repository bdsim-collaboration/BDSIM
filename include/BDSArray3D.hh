#ifndef BDSARRAY3D_H
#define BDSARRAY3D_H

#include "BDSArray4D.hh"
#include "BDSFieldValue.hh"

#include "G4Types.hh"

#include <ostream>

class BDSArray3D: public BDSArray4D
{
public:
  BDSArray3D(G4int nX, G4int nY, G4int nZ);
  virtual ~BDSArray3D(){;}

  /// Setter & (technically, a non-const) accessor.
  virtual BDSFieldValue& operator()(const G4int x,
				    const G4int y,
				    const G4int z,
				    const G4int t);

  /// Accessor only.
  virtual const BDSFieldValue& operator()(const G4int x,
					  const G4int y,
					  const G4int z,
					  const G4int t) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3D const &a);

private:
  BDSArray3D() = delete;
};

#endif
