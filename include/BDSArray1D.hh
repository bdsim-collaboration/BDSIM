#ifndef BDSARRAY1D_H
#define BDSARRAY1D_H

#include "BDSArray2D.hh"
#include "BDSFieldValue.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief 1D array for completeness in array system.
 * 
 * @author Laurie Nevay
 */

class BDSArray1D: public BDSArray2D
{
public:
  BDSArray1D(G4int nX);
  virtual ~BDSArray1D();

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray2D const &a);

private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray1D() = delete;
};

#endif
