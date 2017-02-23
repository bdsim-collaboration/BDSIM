#ifndef BDSARRAY2D_H
#define BDSARRAY2D_H

#include "BDSArray3D.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief 2D array.
 * 
 * Inherits (ie is a type of) BDSArray3D and in turn BDSArray4D.
 *
 * @author Laurie Nevay
 */

class BDSArray2D: public BDSArray3D
{
public:
  BDSArray2D(G4int nXIn, G4int nYIn);
  virtual ~BDSArray2D(){;}
  
  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray2D const &a);
  
private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray2D() = delete;
};

#endif
