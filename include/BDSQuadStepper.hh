#ifndef BDSQUADSTEPPER_HH
#define BDSQUADSTEPPER_HH
#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"

class BDSQuadStepper : public G4MagIntegratorStepper
{

public:  // with description

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

public: // without description
  
  G4int IntegratorOrder()const { return 2; }

protected:
  //  --- Methods used to implement all the derived classes -----

  void AdvanceHelix(const G4double  yIn[],
		    G4ThreeVector Bfld,
		    G4double  h,
		    G4double  yQuad[]);    // output 
  // A first order Step along a quad inside the field.

private:
  /// Keep a local navigator to work out positions and transforms at various points.
  /// If we use the general navigator, then this actually changes the position of the
  /// particle being tracked.
  G4Navigator* QuadNavigator;

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

  /// Record whether the class has been used or not. Use to get the
  /// world volume for a local navigator object on first use. We have
  /// to do this because the stepper objects are typically created before
  /// the world is constructed and registered. However, we don't need to do
  /// this every time - only once.
  G4bool   initialised;
};

inline  void BDSQuadStepper::SetBGrad(G4double aBGrad)
{itsBGrad=aBGrad;}

inline G4double BDSQuadStepper::GetBGrad() const
{return itsBGrad;}

inline void BDSQuadStepper::StepperName()
{G4cout<<"BDSQuadStepper"<<G4endl;}

#endif /* BDSQUADSTEPPER_HH */
