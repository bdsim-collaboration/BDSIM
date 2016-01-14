#ifndef BDSFIELDMAGSBEND_H
#define BDSFIELDMAGSBEND_H

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "BDSFieldMagBase.hh"

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

class BDSFieldMagSBend: public BDSFieldMagBase
{
public:
  /// A constant uniform field with value equal to 'field'. If angle is
  /// finite, the local radius of curvature is calculated and used by 
  BDSFieldMagSBend(G4ThreeVector  fieldIn,
		   const G4double aLength,
		   const G4double aAngle  = 0,
		   G4bool debugIn = false);

  BDSFieldMagSBend(BDSMagnetStrength* strength,
		   G4double           brho,
		   G4ThreeVector      unitDirection = G4ThreeVector(0,0,0));
  
  ~BDSFieldMagSBend(){;}

  /// The method Geant4 will use to access the field value
  virtual void  GetFieldValue(const G4double point[4],
			      G4double* field) const;

  /// Access the local radius of curvature for the nominal momentum in this field.
  G4double GetLocalRadius() const;

private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagSBend();
  
  /// The vector of the field. This constructed in local coordinates and the first time
  /// the field is queried, the auxiliary navigator (through inheritance) is used to
  /// transform the vector to global coordinates.
  G4ThreeVector localField;
  
  /// Radius of curvature.
  G4double localRadius;
};

inline G4double BDSFieldMagSBend::GetLocalRadius() const
{return localRadius;}

#endif
