#ifndef BDSARRAY2D_H
#define BDSARRAY2D_H

#include "globals.hh" // geant4 types / globals

#include <ostream>
#include <vector>

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

template <typename T>
class BDSArray2D
{
public:
  BDSArray2D(G4int nXIn, G4int nYIn):
    nX(nXIn), nY(nYIn),
    data(std::vector<T>(nX*nY))
  {;}

  ~BDSArray2D(){;}

  inline G4int sizeX() const {return nX;}
  inline G4int sizeY() const {return nY;}

  T& operator()(const G4int x, const G4int y)
  {
    // nX,nY are at least 1 but the index would be 0 hence +1
    if ((x+1 > nX) || (y+1 > nY))
      {G4cout << "Index (" << x << "," << y << ") is outside range of array" << G4endl; exit(1);}
    return data[x*nX + y];
  }

  const T& operator()(const G4int x, const G4int y) const
  {
    // nX,nY are at least 1 but the index would be 0 hence +1
    if ((x+1 > nX) || (y+1 > nY))
      {G4cout << "Index (" << x << "," << y << ") is outside range of array" << G4endl; exit(1);}
    return data[x*nX + y];
  }

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray2D const &a)
  {
    out << "#x = " << a.nX << ", ";
    out << "#y = " << a.nY << G4endl;

    for (G4int y = 0; y < a.nY; y++)
      {
	out << "[ ";
	for (G4int x = 0; x < a.nX; x++)
	  {out << a(x,y) << "\t";}
	out << "]" << G4endl;
      }
    return out;
  }
  
private:
  /// Private default constructor to force use of provided one.
  BDSArray2D();

  /// Number of entries in 0th dimension - x (at least 1).
  G4int nX;

  /// Number of entries in 1st dimension - y (at least 1).
  G4int nY;

  /// Data for array in 1D vector.
  std::vector<T> data;
};

#endif
