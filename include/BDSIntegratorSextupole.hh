#ifndef BDSINTEGRATORSEXTUPOLE_H
#define BDSINTEGRATORSEXTUPOLE_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

class G4MagIntegratorStepper;

/**
 * @brief Integrator for sextupole field.
 */

class BDSIntegratorSextupole: public BDSIntegratorMag
{
public:
  BDSIntegratorSextupole(BDSMagnetStrength const* strength,
			 G4double                 brho,
			 G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorSextupole(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);
  
protected:
  /// Calculate the new particle coordinates. A first order Step along a solenoid
  /// inside the field.
  void AdvanceHelix(const G4double yIn[],
		    G4double       h,
		    G4double       yOut[]);

private:
  /// 2nd derivative of the field
  G4double bDoublePrime;
};

#endif
