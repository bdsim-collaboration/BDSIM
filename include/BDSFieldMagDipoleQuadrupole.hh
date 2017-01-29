#ifndef BDSFIELDMAGDIPOLEQUADRUPOLE_H
#define BDSFIELDMAGDIPOLEQUADRUPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;
class BDSFieldMagSBend;
class BDSFieldMagDipoleQuadrupole;
class BDSFieldMagQuadrupole;

/**
 * @brief Class that provides the magnetic strength in a mixed dipole / quadrupole.
 * 
 * This is the linear sum of a dipole and quadrupolar field (as defined by k1).
 *
 * @author Laurie Nevay
 */

class BDSFieldMagDipoleQuadrupole: public BDSFieldMag
{
public:
  BDSFieldMagDipoleQuadrupole(BDSMagnetStrength const* strength,
			      G4double          const  brho);
  
  virtual ~BDSFieldMagDipoleQuadrupole();

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;

  virtual void SetTransform(const G4Transform3D& transformIn);
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagDipoleQuadrupole();
  
  BDSFieldMagQuadrupole* quad;
  BDSFieldMagSBend*      dipole;
};

#endif
