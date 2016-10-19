#ifndef BDSINTERPOLATORROUTINES_H
#define BDSINTERPOLATORROUTINES_H

#include "BDSFieldValue.hh"

#include "G4Types.hh"

namespace BDS
{
  /// Interpolate between to field values for a given normalised distance between
  /// 0,1.
  BDSFieldValue Linear1D(const BDSFieldValue& v1,
			 const BDSFieldValue& v2,
			 const G4double       dist);

  /// Cubic interpolation in 1 dimension.
  BDSFieldValue Cubic1D(const BDSFieldValue p[4],
			const G4double      x);
  
  /// Cubic interpolation in 2 dimensions.
  BDSFieldValue Cubic2D(const BDSFieldValue p[4][4],
			const G4double      x,
			const G4double      y);

  /// Cubic interpolation in 3 dimensions.
  BDSFieldValue Cubic3D(const BDSFieldValue p[4][4][4],
			const G4double      x,
			const G4double      y,
			const G4double      z);

  /// Cubic interpolation in 4 dimensions.
  BDSFieldValue Cubic4D(const BDSFieldValue p[4][4][4][4],
			const G4double      x,
			const G4double      y,
			const G4double      z,
			const G4double      t);
}

#endif
