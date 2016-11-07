#ifndef BDSTWOVECTOR_H
#define BDSTWOVECTOR_H

#include "globals.hh" // geant4 types / globals

#include <ostream>

/**
 * @brief Simple two vector that's templated so the right type can be used.
 * 
 * Only the assigment operator is provided, other than that it's simple access
 * and setting only.
 * 
 * @author Laurie Nevay
 */

template <typename T>
class BDSTwoVector
{
public:
  /// Default constructor.
  BDSTwoVector()
  {values[0] = 0; values[1] = 0;}

  /// Alternate constructor.
  BDSTwoVector(T x, T y)
  {values[0] = x; values[1] = y;}

  /// Copy constructor.
  BDSTwoVector(const BDSTwoVector& other)
  {
    for (G4int i : {0, 1})
      {values[i] = other.values[i];}
  }

  /// Assignment operator.
  BDSTwoVector& operator=(const BDSTwoVector& rhs)
  {
    values[0] = rhs.values[0];
    values[1] = rhs.values[1];
    return *this;
  }

  /// Access / set a single element.
  T& operator[](const G4int index)
  {
    if (index > 1)
      {G4cerr << "index outside array" << G4endl; exit(1);}
    return values[index];
  }

  // Access a single element.
  const T& operator[](const G4int index) const
  {
    if (index > 1)
      {G4cerr << "index outside array" << G4endl; exit(1);}
    return values[index];
  }

  /// @{ Accessor by name.
  inline const T& x() const {return values[0];}
  inline const T& y() const {return values[1];}
  /// @}
  
  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSTwoVector const &v)
  {out << "(" << v.values[0] << ", " << v.values[1] << ")"; return out;}

private:
  /// Member data - x,y.
  T values[2];
};


#endif
