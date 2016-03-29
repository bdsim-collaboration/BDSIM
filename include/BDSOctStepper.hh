#ifndef BDSOCTSTEPPER_H
#define BDSOCTSTEPPER_H

#include "BDSAuxiliaryNavigator.hh"

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

class BDSOctStepper:
  public G4MagIntegratorStepper, public BDSAuxiliaryNavigator
{
public:
  BDSOctStepper(G4Mag_EqRhs *EqRhs);
  ~BDSOctStepper();

  void Stepper( const G4double y[],
		const G4double dydx[],
		const G4double h,
		G4double yout[],
		G4double yerr[]  );
  // The stepper for the Runge Kutta integration.
  // The stepsize is fixed, equal to h.
  // Integrates ODE starting values y[0 to 6]
  // Outputs yout[] and its estimated error yerr[].

  G4double DistChord()   const;
  // Estimate maximum distance of curved solution and chord ... 
 
  void SetBTrpPrime(G4double aBTrpPrime);
  G4double GetBTrpPrime();

  void StepperName();
  
  G4int IntegratorOrder()const { return 2; }

protected:
  void AdvanceHelix( const G4double  yIn[],
		     G4ThreeVector Bfld,
		     G4double  h,
		     G4double  yOct[]);    // output 
  // A first order Step along a oct inside the field.

private:
  
  G4Mag_EqRhs*  fPtrMagEqOfMot;

  G4double itsBTrpPrime;

  G4ThreeVector yInitial, yMidPoint, yFinal;
  // Data stored in order to find the chord.

  G4double itsDist;
};

inline  void BDSOctStepper::SetBTrpPrime(G4double aBTrpPrime)
{itsBTrpPrime=aBTrpPrime;
}

inline G4double BDSOctStepper::GetBTrpPrime()
{return itsBTrpPrime;}

inline void BDSOctStepper::StepperName()
{G4cout<<"BDSOctStepper"<<G4endl;}

#endif /* BDSOCTSTEPPER_HH */
