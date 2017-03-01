#ifndef BDSINTEGRATORQUADRUPOLE_H
#define BDSINTEGRATORQUADRUPOLE_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution to a quadrupole.
 * 
 * Analytical solution to a quadrupole field. This integrator will use the analytical solution
 * for a quadrupole (matrix) to transport a particle along a given step length. This will only
 * do so for particles that are considered paraxial. For particles that don't meet this criteria
 * the backupStepper from BDSAuxiliaryNavigator is used to integrate through the quadrupolar
 * field. This ensures the integrator functions correctly with particles with large transverse
 * momenta, or even ones that are travelling backwards such as secondaries.
 * 
 */

class BDSIntegratorQuadrupole: public BDSIntegratorMag
{
public:
  BDSIntegratorQuadrupole(BDSMagnetStrength const* strength,
			  G4double                 brho,
			  G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorQuadrupole(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double y[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

protected:
  /// Calculate the new particle coordinates for a given step length h.
  void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    const G4double h,
		    G4double       yOut[],
		    G4double       yErr[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorQuadrupole();
  
  /// B Field Gradient
  G4double bPrime;

  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
