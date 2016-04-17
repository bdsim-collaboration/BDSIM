#ifndef BDSTHREEVECTOR_H
#define BDSTHREEVECTOR_H

#include "globals.hh" // geant4 types / globals

#include <ostream>

/**
 * @brief Simple three vector that's templated so the right type can be used.
 * 
 * The G4ThreeVector(HEP::ThreeVector) is much better, but this only uses
 * a double. Here, a simple three vector that's templated allows the
 * possibility of using float or double precision, which could make a 
 * significant difference to the memory usage - very important for large
 * arrays of this class.
 *
 * Should more complex features be required - it's best to use an actual
 * G4ThreeVector - constructed from this - and then translated back.
 * 
 * @author Laurie Nevay
 */

template <typename T>
class BDSThreeVector
{
public:
  /// Default constructor.
  BDSThreeVector()
  {values[0] = 0; values[1] = 0; values[2] = 0;}

  /// Alternate constructor.
  BDSThreeVector(T x, T y, T z)
  {values[0] = x; values[1] = y; values[2] = z;}

  /// Copy constructor.
  BDSThreeVector(const BDSThreeVector& other)
  {
    for (G4int i : {0, 1, 2})
      {values[i] = other.values[i];}
  }

  /// Assignment operator.
  BDSThreeVector& operator=(const BDSThreeVector& rhs)
  {
    for (G4int i : {0, 1, 2})
      {values[i] = rhs.values[i];}
    return this*;
  }

  /// Access / set a single element.
  T& operator[](const G4int index)
  {
    if (index > 2)
      {G4cerr << "index outside array" << G4endl; exit(1);}
    return &values[index];
  }

  /// Access a single element.
  const T& operator[](const G4int index) const
  {
    if (index > 2)
      {G4cerr << "index outside array" << G4endl; exit(1);}
    return &values[index];
  }

  /// @{ Accessor by name.
  inline const T& x() const {return values[0];}
  inline const T& y() const {return values[1];}
  inline const T& z() const {return values[2];}
  /// @}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSThreeVector const &v)
  {out << "(" << v.values[0] << ", " << v.values[1] << ", " << v.values[2] << ")"; return out;}

private:
  /// Member data - x,y,z.
  T values[3];
};


#endif
