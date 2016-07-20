#ifndef BDSINTEGRATORMULTIPOLE_H
#define BDSINTEGRATORMULTIPOLE_H

#include "BDSIntegratorBase.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include <list>

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

class BDSIntegratorMultipole: public BDSIntegratorBase
{
public:
  BDSIntegratorMultipole(BDSMagnetStrength const* strength,
			  G4double                 brho,
			  G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorMultipole(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double y[],
	      	 const G4double dydx[],
	       const G4double h,
	       G4double       yOut[],
	       G4double       yErr[]);

protected:
  /// Calcaulte the new particle coordinates for a given step length h.
  void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    const G4double h,
		    G4double       yOut[],
		    G4double yErr[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorMultipole();

  G4int Factorial(G4int n);

  /// Dipole component
  G4double b0l;
  /// Higher order components
  std::list<double> bnl;
  std::list<double> bsl;
  std::vector<G4int> nfact;

  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
