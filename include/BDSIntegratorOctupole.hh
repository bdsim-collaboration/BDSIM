#ifndef BDSINTEGRATOROCTUPOLE_H
#define BDSINTEGRATOROCTUPOLE_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/** 
 * @brief Integrator for octupole field.
 */

class BDSIntegratorOctupole: public BDSIntegratorMag
{
public:
  BDSIntegratorOctupole(BDSMagnetStrength const* strength,
			G4double                 brho,
			G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorOctupole(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

protected:
  /// Calculate the new particle coordinates.
  void AdvanceHelix(const G4double yIn[],
		    G4double       h,
		    G4double       yOut[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorOctupole();

  G4double bTriplePrime;
  
  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
