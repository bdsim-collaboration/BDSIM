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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef BDSSteppingAction_h
#define BDSSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class BDSSteppingAction : public G4UserSteppingAction
{
public:
  BDSSteppingAction();
  virtual ~BDSSteppingAction();

  virtual void UserSteppingAction(const G4Step*);

private:
  void ThresholdCutSteppingAction();
  void VerboseSteppingAction();
  const G4Step* _step;
};

#endif
