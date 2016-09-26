#ifndef BDSARRAY2D_H
#define BDSARRAY2D_H

#include "BDSArray3D.hh"
#include "BDSFieldValue.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief A 2D array managed interally as a 1D array.
 * 
 * This stores data by index only, not in real spatial coordinates.
 * The dimensions of the array must be known at construction time.
 *
 * A templated array class that dynamically stores data using a 
 * vector. The class provides an interface such that a 2D array 
 * is really stored as a 1D array. If using a raw array, the compiler
 * would provide such an interface, but here it is provided explicitly
 * to avoid memory management issues with arrays as well as possible
 * faults due to duplication.
 *
 * This methodology has the advantage that the data is contiguous in 
 * memory and therefore more efficiently copied, where as a vector of
 * vectors is really just a large array of pointers.
 * 
 * The () operator is used for accessing and setting as the [] operator
 * can only ever accept one parameter by design.
 * https://isocpp.org/wiki/faq/operator-overloading#matrix-subscript-op
 * 
 * array(indx,indy) -> array.data[indx*nX + nY]
 * 
 * This class is templated so any type can be used, but the type must implement
 * std::ostream operator<<.
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
  /// Private default constructor to force use of provided one.
  BDSArray2D() = delete;
};

#endif
