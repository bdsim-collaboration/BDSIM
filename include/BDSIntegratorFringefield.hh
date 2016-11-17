#ifndef BDSINTEGRATORFRINGEFIELD_H
#define BDSINTEGRATORFRINGEFIELD_H

#include "BDSIntegratorDipole.hh"
#include "BDSIntegratorDipole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Integrator that ignores the field and uses the analytical solution for a dipole kick.
 *
 * @author Will Shields 
 */

class BDSIntegratorFringefield: public BDSIntegratorDipole
{
public:
  BDSIntegratorFringefield(BDSMagnetStrength const* strength,
			   G4double                 brho,
			   G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorFringefield(){;}

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
  virtual void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    const G4double h,
		    G4double       yOut[],
		    G4double       yErr[]);
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorFringefield();
  
  /// Poleface rotation angle
  G4double polefaceAngle;
  /// Fringe field correction term
  G4double fringeCorr;
  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
