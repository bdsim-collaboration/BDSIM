/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSDriftStepper.hh,v 1.2 2007/11/14 12:57:06 malton Exp $
// GEANT4 tag $Name:  $
//
//
// class BDSDriftStepper
//
// Class description:
//

// History:
// - Created. G.Blair 1/11/00

#ifndef BDSDRIFTSTEPPER_HH
#define BDSDRIFTSTEPPER_HH
#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
//#include "G4MagHelicalStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"

class BDSDriftStepper : public G4MagIntegratorStepper
{

public:  // with description
  
  BDSDriftStepper(G4Mag_EqRhs *EqRhs);
  
  ~BDSDriftStepper();
  
  void Stepper( const G4double y[],
		const G4double dydx[],
		const G4double h,
		G4double yout[],
		G4double yerr[]  );
  // The stepper for the Runge Kutta integration.
  // The stepsize is fixed, equal to h.
  // Integrates ODE starting values y[0 to 6]
  // Outputs yout[] and its estimated error yerr[].
  
  
  void StepperName();
  
  G4double DistChord()const;
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
		     G4ThreeVector Bfld,
		     G4double  h,
		     G4double  yDrift[]);    // output 
  // A first order Step along a drift inside the field.
  
private:
  G4bool verboseStep;
  G4int  verboseEventNumber;
  
};


inline void BDSDriftStepper::StepperName()
{G4cout<<"BDSDriftStepper"<<G4endl;}

#endif /* BDSDRIFTSTEPPER_HH */
