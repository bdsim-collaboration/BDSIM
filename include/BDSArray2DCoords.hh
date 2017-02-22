#ifndef BDSARRAY2DCOORDS_H
#define BDSARRAY2DCOORDS_H

#include "BDSArray3DCoords.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief 2D array with spatial mapping derived from BDSArray4DCoords.
 *
 * @author Laurie Nevay
 */

class BDSArray2DCoords: public BDSArray3DCoords
{
public:
  BDSArray2DCoords(G4int nX, G4int nY, G4int loopOrder,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn);
  virtual ~BDSArray2DCoords(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray2DCoords const &a);

private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray2DCoords() = delete;
};

#endif
