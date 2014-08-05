/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 18.10.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/


#ifndef BDSHELIXSTEPPER_HH
#define BDSHELIXSTEPPER_HH
#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"
#include "G4Mag_EqRhs.hh"


class BDSHelixStepper : public G4MagIntegratorStepper
{

  public:  // with description

  BDSHelixStepper(G4Mag_EqRhs *EqRhs);

  ~BDSHelixStepper();

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
  
  void SetBField(G4double aBfield);
  G4double GetBField();

  void StepperName();
  
  /*
    void DumbStepper( const G4double yInput[],
    G4ThreeVector Bfld,
    G4double hstep,
    G4double yOut[]);
    
  */
public: // without description
  
  G4int IntegratorOrder()const { return 2; }
  
protected:
  //  --- Methods used to implement all the derived classes -----
  
  void AdvanceHelix( const G4double  yIn[],
		     G4double  h,
		     G4double  yHelix[]);    // output 
  // A first order Step along a helix inside the field.
  
private:
  
  G4ThreeVector yInitial, yMidPoint, yFinal;
  // Data stored in order to find the chord.
  
  G4Mag_EqRhs*  its_EqRhs;
  
  G4double itsBField;
  G4ThreeVector itsInitialPoint, itsFinalPoint, itsMidPoint,itsDistVec;
  G4double itsDist;
  
};

inline  void BDSHelixStepper::SetBField(G4double aBField)
{itsBField=aBField;
}
inline G4double BDSHelixStepper::GetBField()
{return itsBField;}

inline void BDSHelixStepper::StepperName()
{G4cout<<"BDSHelixStepper"<<G4endl;}

#endif /* BDSHELIXSTEPPER_HH */
