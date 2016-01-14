#ifndef BDSINTEGRATORSEXTUPOLE_H
#define BDSINTEGRATORSEXTUPOLE_H

#include "BDSIntegratorBase.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

class BDSIntegratorSextupole: public BDSIntegratorBase
{
public:
  BDSIntegratorSextupole(const BDSMagnetStrength* strength,
			 const G4double           brho,
			 G4Mag_EqRhs* const       eqRHSIn);
  
  ~BDSIntegratorSextupole(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  void Stepper(const G4double y[],
	       const G4double dydx[],
	       const G4double h,
	       G4double yout[],
	       G4double yerr[]);

protected:
  /// Calcaulte the new particle coordinates. A first order Step along a solenoid inside the field.
  void AdvanceHelix( const G4double  yIn[],
		     G4ThreeVector Bfld,
		     G4double  h,
		     G4double  ySext[]);

private:
  /// 2nd derivative of the field
  G4double bDoublePrime;

  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
