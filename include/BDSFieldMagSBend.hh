#ifndef BDSFIELDMAGSBEND_H
#define BDSFIELDMAGSBEND_H

#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief A uniform dipole field.
 * 
 * This class represents a uniform dipole field. This may optionally be constructed
 * with a unit vector to denote a different direction. This uses the parameters
 * "angle" and "length" (the magnetic field length) from the input 
 * BDSMagnetStrength instance. MADX uses angle rather than k0, so we follow that
 * convention.
 */

class BDSFieldMagSBend: public BDSFieldMag
{
public:
  /// A constant uniform field with value equal to 'field'. If angle is
  /// finite, the local radius of curvature is calculated and used by 
  BDSFieldMagSBend(G4ThreeVector  fieldIn,
		   const G4double aLength,
		   const G4double aAngle  = 0,
		   G4bool debugIn = false);

  BDSFieldMagSBend(BDSMagnetStrength const* strength,
		   G4double          const  brho,
		   G4ThreeVector unitDirection = G4ThreeVector(0,1,0));

  virtual ~BDSFieldMagSBend(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;

private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagSBend();
  
  /// The vector of the field. This constructed in local coordinates and the first time
  /// the field is queried, the auxiliary navigator (through inheritance) is used to
  /// transform the vector to global coordinates.
  G4ThreeVector localField;
  
};

#endif
