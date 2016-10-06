#ifndef BDSARRAY2DCOORDS_H
#define BDSARRAY2DCOORDS_H

#include "BDSArray3DCoords.hh"

#include "globals.hh"

#include <ostream>

class BDSArray2DCoords: public BDSArray3DCoords
{
public:
  BDSArray2DCoords(G4int nX, G4int nY,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn);
  virtual ~BDSArray2DCoords(){;}

  friend std::ostream& operator<< (std::ostream& out, BDSArray2DCoords const &a);

private:
  BDSArray2DCoords() = delete;
};

#endif
