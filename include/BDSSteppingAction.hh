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
// $Id: BDSSteppingAction.hh,v 1.3 2007/07/18 13:35:01 malton Exp $
// GEANT4 tag $Name:  $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef BDSSteppingAction_h
#define BDSSteppingAction_h 1

#include <vector>

#include "G4UserSteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class BDSSteppingAction : public G4UserSteppingAction
{
public:
  BDSSteppingAction();
  virtual ~BDSSteppingAction();

  virtual void UserSteppingAction(const G4Step*);

  void SetTrackLengthInWorldRegion(G4double dvalue);
  void SetTrackLength(G4double dvalue);
  G4double GetTrackLengthInWorldRegion();
  G4double  GetTrackLength();

private:
  G4bool verbose;     
  G4bool verboseStep;
  G4bool verboseEvent;
  G4int  verboseEventNumber;
  G4int  nptwiss;

  //  G4double itsTrackWeight;
  //  G4double itsInverseTrackWeight;

  //  G4int itsLastTrackID,itsNtry,itsNmax;
  //  G4double itsLastZpos;
  //  G4double itsZposTolerance;
  //  G4double itsPosKick;
  G4double postponedEnergy;

  G4double trackLengthInWorldRegion;
  G4double trackLength;

  std::vector<G4ThreeVector> r;
  std::vector<G4ThreeVector> rp;
  

 };

#endif
