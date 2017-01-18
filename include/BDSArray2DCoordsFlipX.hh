#ifndef BDSARRAY2DCOORDSFLIPX_H
#define BDSARRAY2DCOORDSFLIPX_H

#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief A wrapper to achieve reflection about the X axis.
 *
 * This owns the array it wraps.
 * 
 * \verbatim
 * ∧
 * |
 * --->
 * ========> x
 * --->
 * |
 * ∨
 * \endverbatim
 *
 * This makes an array look 2 times bigger than it is - ie 2x as big in y. The
 * array coordinates are correspondingly shifted. What would've been (0,0) in a 100x100
 * array will now be at index (101,101).
 *
 * This is reflected about 0,0 and can only be so given the implementation.
 *
 * Some interfaces are overloaded and some aren't as are required to be (sizeY for example).
 * The ostream << writes both the raw array and the reflected version too.
 *
 * Note, by return a reference to a non-const member returnValue that may be updated by another
 * query, thread safety could be an issue.
 * 
 * @author Laurie Nevay
 */

class BDSArray2DCoordsFlipX: public BDSArray2DCoords
{
public:
  /// Wrap an existing BDSArray2DCoords instance.
  BDSArray2DCoordsFlipX(BDSArray2DCoords* arrayIn);
  virtual ~BDSArray2DCoordsFlipX(){;}

  /// @{ Overridden from BDSArray4DCoords.
  virtual G4bool OutsideCoords(const G4double x,
			       const G4double y,
			       const G4double z,
			       const G4double t) const;
  
  virtual G4double ArrayCoordsFromY(const G4double y) const;
  virtual G4int    NearestY(const G4double y) const;
  /// @}

  /// @{ Overridden from BDSArray4D.
  virtual const BDSFieldValue& GetConst(const G4int x,
					const G4int y,
					const G4int z = 0,
					const G4int t = 0) const;

  virtual G4bool Outside(const G4int x,
			 const G4int y,
			 const G4int z,
			 const G4int t) const;
  /// @}

  /// This prints out the raw underlying data, then the reflected version as would normally
  /// be queried.
  virtual std::ostream& Print(std::ostream& out) const;

  /// Delegate function to call polymorphic Print().
  friend std::ostream& operator<< (std::ostream& out, BDSArray2DCoordsFlipX const &a);

private:
  BDSArray2DCoordsFlipX() = delete;

  /// Must return by reference and the true data value may have to be reflected
  /// in one or more dimensions.  Copy to this and return reference to it.
  mutable BDSFieldValue returnValue;
};

#endif
