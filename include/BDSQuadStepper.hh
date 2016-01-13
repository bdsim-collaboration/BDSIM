#ifndef BDSQUADSTEPPER_H
#define BDSQUADSTEPPER_H

#include "BDSAuxiliaryNavigator.hh"

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

class BDSQuadStepper:
  public G4MagIntegratorStepper, public BDSAuxiliaryNavigator
{
public:
  BDSQuadStepper(G4Mag_EqRhs *EqRhs);
  ~BDSQuadStepper();

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
 
  void     SetBGrad(G4double aBGrad);
  G4double GetBGrad() const;

  void StepperName();
  
  G4int IntegratorOrder()const { return 2; }

protected:
  void AdvanceHelix(const G4double  yIn[],
		    G4ThreeVector Bfld,
		    G4double  h,
		    G4double  yQuad[]);    // output 
  // A first order Step along a quad inside the field.

private:
  /// Charge
  G4Mag_EqRhs* fPtrMagEqOfMot;

  /// B Field Gradient
  G4double itsBGrad;

  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;

  /// Local variable used to passs information to DistChord() which gives the distance
  /// from the chord calculated by the in and out points and the true curved path the
  /// particle would take in the field.
  G4double itsDist;
};

inline  void BDSQuadStepper::SetBGrad(G4double aBGrad)
{itsBGrad=aBGrad;}

inline G4double BDSQuadStepper::GetBGrad() const
{return itsBGrad;}

inline void BDSQuadStepper::StepperName()
{G4cout<<"BDSQuadStepper"<<G4endl;}

#endif /* BDSQUADSTEPPER_HH */
