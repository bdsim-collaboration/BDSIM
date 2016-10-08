#ifndef BDSARRAY1DCOORDS_H
#define BDSARRAY1DCOORDS_H

#include "BDSArray2DCoords.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief 1D array with spatial mapping derived from BDSArray4DCoords.
 *
 * @author Laurie Nevay
 */

class BDSArray1DCoords: public BDSArray2DCoords
{
public:
  BDSArray1DCoords(G4int nX,
		   G4double xMinIn, G4double xMaxIn);
  virtual ~BDSArray1DCoords(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray1DCoords const &a);

private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray1DCoords() = delete;
};

#endif
