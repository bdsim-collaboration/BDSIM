#ifndef BDSARRAY3DCOORDS_H
#define BDSARRAY3DCOORDS_H

#include "BDSArray4DCoords.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief 3D array with spatial mapping derived from BDSArray4DCoords.
 *
 * @author Laurie Nevay
 */

class BDSArray3DCoords: public BDSArray4DCoords
{
public:
  BDSArray3DCoords(G4int nX, G4int nY, G4int nZ, G4int loopOrder,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn);
  virtual ~BDSArray3DCoords(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3DCoords const &a);

private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray3DCoords() = delete;
};

#endif
