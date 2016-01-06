#ifndef BDSSEXTSTEPPER_H
#define BDSSEXTSTEPPER_H

#include "BDSAuxiliaryNavigator.hh"

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

class BDSSextStepper:
  public G4MagIntegratorStepper, public BDSAuxiliaryNavigator
{
public:
  BDSSextStepper(G4Mag_EqRhs *EqRhs);
  ~BDSSextStepper();

  void Stepper( const G4double y[],
		const G4double dydx[],
		const G4double h,
		G4double yout[],
		G4double yerr[]  );
  // The stepper for the Runge Kutta integration.
  // The stepsize is fixed, equal to h.
  // Integrates ODE starting values y[0 to 6]
  // Outputs yout[] and its estimated error yerr[].

  G4double DistChord() const;
  // Estimate maximum distance of curved solution and chord ... 
 
  void SetBDblPrime(G4double aBDblPrime);
  G4double GetBDblPrime();

  void StepperName();
  
  G4int IntegratorOrder()const { return 2; }

protected:
  void AdvanceHelix( const G4double  yIn[],
		     G4ThreeVector Bfld,
		     G4double  h,
		     G4double  ySext[]);    // output 
  // A first order Step along a sext inside the field.

private:
  G4Mag_EqRhs*  fPtrMagEqOfMot;

  G4double itsBDblPrime;

  G4ThreeVector yInitial, yMidPoint, yFinal;
  // Data stored in order to find the chord.

  G4double itsDist;

};

inline  void BDSSextStepper::SetBDblPrime(G4double aBDblPrime)
{itsBDblPrime=aBDblPrime;}

inline G4double BDSSextStepper::GetBDblPrime()
{return itsBDblPrime;}

inline void BDSSextStepper::StepperName()
{G4cout<<"BDSSextStepper"<<G4endl;}

#endif /* BDSSEXTSTEPPER_HH */
