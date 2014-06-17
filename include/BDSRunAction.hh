/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSRunAction_h
#define BDSRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "time.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class G4Run;

class BDSRunAction : public G4UserRunAction
{
public:
  BDSRunAction();
  ~BDSRunAction();
  
public:
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:
  time_t starttime;
  time_t stoptime;
};

#endif

