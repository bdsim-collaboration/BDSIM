#ifndef BDSEXTENT_H
#define BDSEXTENT_H

#include <utility>

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

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
  inline std::pair<G4double, G4double> ExtentX() const {return extentX;}
  inline std::pair<G4double, G4double> ExtentY() const {return extentY;}
  inline std::pair<G4double, G4double> ExtentZ() const {return extentZ;}

  inline G4double XPos() const {return extentX.second;}
  inline G4double XNeg() const {return extentX.first;}
  inline G4double YPos() const {return extentY.second;}
  inline G4double YNeg() const {return extentY.first;}
  inline G4double ZPos() const {return extentZ.second;}
  inline G4double ZNeg() const {return extentZ.first;}
  
  inline G4ThreeVector ExtentPositive() const
  {return G4ThreeVector(extentX.second, extentY.second, extentZ.second);}

  inline G4ThreeVector ExtentNegative() const
  {return G4ThreeVector(extentX.first, extentY.first, extentZ.first);}
  /// @}
  
private:
  /// @{ Negative and positive extent
  std::pair<G4double, G4double> extentX;
  std::pair<G4double, G4double> extentY;
  std::pair<G4double, G4double> extentZ;
  /// @}
};

#endif
