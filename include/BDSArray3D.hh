#ifndef BDSARRAY3D_H
#define BDSARRAY3D_H

#include "globals.hh" // geant4 types / globals

#include <ostream>
#include <vector>

/**
 * @brief A 3D array managed interally as a 1D array.
 * 
 * This stores data by index only, not in real spatial coordinates.
 * The dimensions of the array must be known at construction time.
 *
 * A templated array class that dynamically stores data using a 
 * vector. The class provides an interface such that a 3D array 
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
 * array(indx,indy,indz) -> array.data[indx*nX indy*nY + nZ]
 * 
 * This class is templated so any type can be used, but the type  must implement
 * std::ostream operator<<.
 *
 * @author Laurie Nevay
 */

template <typename T>
class BDSArray3D
{
public:
  BDSArray3D(G4int nXIn, G4int nYIn, G4int nZIn):
    nX(nXIn), nY(nYIn), nZ(nZIn)
    data(std::vector<T>(nX*nY*nZ))
  {;}

  ~BDSArray3D(){;}

  inline G4int sizeX() const {return nX;}
  inline G4int sizeY() const {return nY;}
  inline G4int sizeZ() const {return nZ;}

  T& operator()(const G4int x, const G4int y, const G4int z)
  {
    // nX,nY,nZ are at least 1 but the index would be 0 hence +1
    if ((x+1 > nX) || (y+1 > nY) || (z+1 > nZ))
      {G4cout << "Index (" << x << "," << y << "," << z << ") is outside range of array" << G4endl; exit(1);}
    return data[x*nX + y*nY + z];
  }

  const T& operator()(const G4int x, const G4int y, const z) const
  {
    // nX,nY are at least 1 but the index would be 0 hence +1
    if ((x+1 > nX) || (y+1 > nY) || (z+1 > nZ))
      {G4cout << "Index (" << x << "," << y << "," << z << ") is outside range of array" << G4endl; exit(1);}
    return data[x*nX + y];
  }

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3D const &a)
  {
    out << "#x = " << a.nX << ", ";
    out << "#y = " << a.nY << ", ";
    out << "#z = " << a.nZ << G4endl;

    for (G4int z = 0; z < z.nZ; z++)
      {
	out << "[ " << G4endl;
	for (G4int y = 0; y < a.nY; y++)
	  {
	    out << "[ ";
	    for (G4int x = 0; x < a.nX; x++)
	      {out << a(x,y) << "\t";}
	    out << "]" << G4endl;
	  }
	out << G4endl << "]" << G4endl;
      }
    return out;
  }
  
private:
  /// Private default constructor to force use of provided one.
  BDSArray3D();

  G4int nX; ///< Number of entries in 0th dimension - x (at least 1).
  G4int nY; ///< Number of entries in 1st dimension - y (at least 1).
  G4int nZ; ///< Number of entries in 1st dimension - z (at least 1).

  /// Data for array in 1D vector.
  std::vector<T> data;
};

#endif
