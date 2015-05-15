/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

#ifndef BDSTerminatorUserLimits_h
#define BDSTerminatorUserLimits_h

#include "G4UserLimits.hh"
#include "globals.hh"  // geant4 basic stl types


class BDSTerminatorUserLimits : public G4UserLimits
{
public:
  BDSTerminatorUserLimits(G4double ustepMax = DBL_MAX,
			  G4double utrakMax = DBL_MAX,
			  G4double utimeMax = DBL_MAX,
			  G4double uekinMin = 0.,     
			  G4double urangMin = 0.);    
  
  BDSTerminatorUserLimits(const G4String& type,
			  G4double ustepMax = DBL_MAX,
			  G4double utrakMax = DBL_MAX,
			  G4double utimeMax = DBL_MAX,
			  G4double uekinMin = 0.,
			  G4double urangMin = 0.);
  
  virtual ~BDSTerminatorUserLimits(){};
  //must have implementation otherwise linker error

public:
  // replace this function alone in G4UserLimits
  // where we only set the limit to E=0 eV on the
  // last turn
  virtual G4double GetUserMinEkine(const G4Track&);
  G4bool verbose;

protected:
  G4double keeprunningEK; // minimum energy particle must have to keep going
  G4double stoprunningEK; // same, so everything < DBL_MAX so everything stopped
};

#endif
