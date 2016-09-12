#ifndef BDSEXTENT_H
#define BDSEXTENT_H

#include <ostream>
#include <tuple>   // for std::tie
#include <utility> // for pair and pair relational operators

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSTiltOffset;

/**
 * @brief Holder for +- extents in 3 dimensions.
 * 
 * @author Laurie Nevay
 */

class BDSExtent
{
public:
  /// Default constructor gives 0 in all extents - typically unphysical.
  BDSExtent();
  /// Construct with paris of extents
  BDSExtent(std::pair<G4double, G4double> extXIn,
	    std::pair<G4double, G4double> extYIn,
	    std::pair<G4double, G4double> extZIn);
  /// Construct with individual extents
  BDSExtent(G4double extXNegIn, G4double extXPosIn,
	    G4double extYNegIn, G4double extYPosIn,
	    G4double extZNegIn, G4double extZPosIn);
  /// Symmetric variant - assumed to be +/- same amount
  BDSExtent(G4double extXIn, G4double extYIn, G4double extZIn);
	    
  ~BDSExtent();

  /// @{ Accessor
  inline std::pair<G4double, G4double> ExtentX() const {return std::make_pair(extXNeg, extXPos);}
  inline std::pair<G4double, G4double> ExtentY() const {return std::make_pair(extYNeg, extYPos);}
  inline std::pair<G4double, G4double> ExtentZ() const {return std::make_pair(extZNeg, extZPos);}

  inline G4double XPos() const {return extXNeg;}
  inline G4double XNeg() const {return extXPos;}
  inline G4double YPos() const {return extYNeg;}
  inline G4double YNeg() const {return extYPos;}
  inline G4double ZPos() const {return extZNeg;}
  inline G4double ZNeg() const {return extZPos;}
  
  inline G4ThreeVector ExtentPositive() const
  {return G4ThreeVector(extXPos, extYPos, extZPos);}

  inline G4ThreeVector ExtentNegative() const
  {return G4ThreeVector(extXNeg, extYNeg, extZNeg);}
  /// @}

  /// @{ Comparison operator.
  inline G4bool operator< (const BDSExtent& r) const;
  inline G4bool operator> (const BDSExtent& r) const {return r < (*this);}
  inline G4bool operator<=(const BDSExtent& r) const {return !((*this) > r);}
  inline G4bool operator>=(const BDSExtent& r) const {return !((*this) < r);}
  /// @}

  /// @{ Equality operator.
  inline G4bool operator==(const BDSExtent& r) const;
  inline G4bool operator!=(const BDSExtent& r) const {return !((*this) == r);}
  /// @}

  /// @{ Comparison operator for x,y only. Ignores z (length).
  inline G4bool TransverselyLessThan(const BDSExtent& r) const;
  inline G4bool TransverselyGreaterThan(const BDSExtent& r)   const {return !(this->TransverselyLessThan(r));}
  inline G4bool TransverselyLessEquals(const BDSExtent& r)    const {return !(this->TransverselyGreaterThan(r));}
  inline G4bool TransverselyGreaterEquals(const BDSExtent& r) const {return !(this->TransverselyLessThan(r));}
  /// @}

  /// Provide a new copy of this extent but rotated along Z by a given tilt angle.
  BDSExtent Tilted(G4double angle) const;

  /// @{ Provide a new copy of this extent with an offset applied.
  BDSExtent Offset(G4ThreeVector offset) const;
  BDSExtent Offset(G4double dx, G4double dy, G4double dz) const;
  /// @}

  /// Provide a new copy of this extent with both a tilt and an offset applied.
  BDSExtent TiltOffset(const BDSTiltOffset* tiltOffset) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream &out, BDSExtent const &ext);

  /// Return a copy of this extent shifted in x and y by a given amount.
  BDSExtent Shift(G4double x, G4double y) const;

  BDSExtent ShiftX(G4double x) const;  ///< Return a copy of this extent shift in x only.
  BDSExtent ShiftY(G4double y) const;  ///< Return a copy of this extent shift in y only.

  /// Return the maximum absolute value considering all dimensions.
  G4double MaximumAbs() const;
  
private:
  /// @{ Extent.
  G4double extXNeg;
  G4double extXPos;
  G4double extYNeg;
  G4double extYPos;
  G4double extZNeg;
  G4double extZPos;
  /// @}
};

inline G4bool BDSExtent::operator<(const BDSExtent& r) const
{
  G4bool xOK = (extXNeg > r.extXNeg) && (extXPos < r.extXPos);
  G4bool yOK = (extYNeg > r.extYNeg) && (extYPos < r.extYPos);
  G4bool zOK = (extZNeg > r.extZNeg) && (extZPos < r.extZPos);
  return xOK && yOK && zOK;
}

inline G4bool BDSExtent::operator==(const BDSExtent& r) const
{
  G4bool xOK = (extXNeg == r.extXNeg) && (extXPos == r.extXPos);
  G4bool yOK = (extYNeg == r.extYNeg) && (extYPos == r.extYPos);
  G4bool zOK = (extZNeg == r.extZNeg) && (extZPos == r.extZPos);
  return xOK && yOK && zOK;
}

inline G4bool BDSExtent::TransverselyLessThan(const BDSExtent& r) const
{
  G4bool xOK = (extXNeg > r.extXNeg) && (extXPos < r.extXPos);
  G4bool yOK = (extYNeg > r.extYNeg) && (extYPos < r.extYPos);
  return xOK && yOK;
}

#endif
