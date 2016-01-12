#ifndef BDSINTEGRATORQUADRUPOLE_H
#define BDSINTEGRATORQUADRUPOLE_H

#include "BDSIntegratorBase.hh"

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

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

class BDSIntegratorQuadrupole: public BDSIntegratorBase
{
public:
  BDSIntegratorQuadrupole(const BDSMagnetStrength* strength,
			  const G4double           brho,
			  const G4Mag_EqRhs*       eqRHSIn);
  
  ~BDSIntegratorQuadrupole(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  void Stepper(const G4double y[],
	       const G4double dydx[],
	       const G4double h,
	       G4double yout[],
	       G4double yerr[]);

  /// Estimate maximum distance of curved solution and chord.
  inline G4double DistChord() const;

  /// Geant4 requires that the integrator order must be supplied by the derived class.
  inline virtual G4int IntegratorOrder() const;

protected:
  /// Calcaulte the new particle coordinates. A first order Step along a solenoid inside the field.
  void AdvanceHelix(const G4double  yIn[],
		    G4ThreeVector   Bfld,
		    G4double        h,
		    G4double        yQuad[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorQuadrupole();
  
  /// B Field Gradient
  G4double bPrime;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChord;

  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};
  
inline G4double BDSIntegratorQuadrupole::DistChord() const
{return distChord;}

inline G4int BDSIntegratorQuadrupole::IntegratorOrder() const
{return 2;}

#endif
