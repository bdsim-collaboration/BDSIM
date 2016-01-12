#ifndef BDSINTEGRATORSOLENOID_H
#define BDSINTEGRATORSOLENOID_H

#include "BDSIntegratorBase.hh"

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Integrator that ignores the field and uses the analytical solution to a solenoid.
 * 
 * Analytical solution to a solenoid field.  This is implemented with some caveats - this is
 * only really valid for one complete step through the solenoid. If this is used for a partial
 * step, the edge focussing won't be correct.  A work in progress.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorSolenoid: public BDSIntegratorBase
{
public:
  BDSIntegratorSolenoid(const BDSMagnetStrength* strength,
			const G4double           brho,
			const G4Mag_EqRhs*       eqRHSIn);

  ~BDSIntegratorSolenoid(){;}

  /// The stepper for the Runge Kutta integration. The stepsize is fixed, equal to h.
  /// Integrates ODE starting values y[0 to 6] and utputs yout[] and its estimated error yerr[].
  /// This is a delegate function that may use the BDSIntegratorSolenoid::AdvanceHelix method.
  /// The reason for this is so that intermediate steps can be calculated and therefore the
  /// error ascertained or distance from the chord.  Error calculation is not currently implemented.
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
		    const G4double dydx[],
		    G4ThreeVector Bfld,
		    G4double  h,
		    G4double  yOut[], // output 
		    G4double  yErr[]);    


private:
  /// Private default constructor to enforce use of supplied constructor.
  BDSIntegratorSolenoid();

  /// The field strength.
  G4double bField;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChord;
};

inline G4double BDSIntegratorSolenoid::DistChord() const
{return distChord;}

inline G4int BDSIntegratorSolenoid::IntegratorOrder() const
{return 2;}

#endif
