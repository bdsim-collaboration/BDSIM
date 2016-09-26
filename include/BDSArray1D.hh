#ifndef BDSARRAY1D_H
#define BDSARRAY1D_H

#include "BDSArray2D.hh"
#include "BDSFieldValue.hh"

#include "G4Types.hh"

#include <ostream>

class BDSArray1D: public BDSArray2D
{
public:
  BDSArray1D(G4int nX);
  virtual ~BDSArray1D();

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
  friend std::ostream& operator<< (std::ostream& out, BDSArray2D const &a);

private:
  /// Private default constructor to force use of provided one.
  BDSArray1D() = delete;
};

#endif
