#ifndef BDSARRAY1DCOORDS_H
#define BDSARRAY1DCOORDS_H

#include "BDSArray2DCoords.hh"

#include "globals.hh"

#include <ostream>

class BDSArray1DCoords: public BDSArray2DCoords
{
public:
  BDSArray1DCoords(G4int nX,
		   G4double xMinIn, G4double xMaxIn);
  virtual ~BDSArray1DCoords(){;}

  friend std::ostream& operator<< (std::ostream& out, BDSArray1DCoords const &a);
};

#endif
