#ifndef BDSARRAY3D_H
#define BDSARRAY3D_H

#include "BDSArray4D.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief 3D array class.
 * 
 * All functionality is provided through BDSArray4D.  A 3D array is a 
 * subset of a 4D array.
 * 
 * @author Laurie Nevay
 */

class BDSArray3D: public BDSArray4D
{
public:
  BDSArray3D(G4int nX, G4int nY, G4int nZ);
  virtual ~BDSArray3D(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3D const &a);

private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray3D() = delete;
};

#endif
