/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

/*

This class is to give dynamic user limits to a certain volume.
In the case of a ring, we want to cut ALL particles at the end 
of the ring after a certain number of turns.
*/

#include "G4ios.hh"
#include "BDSTerminatorUserLimits.hh"
#include "G4UserLimits.hh"
#include "BDSGlobalConstants.hh"
#include "BDSExecOptions.hh"
#include "G4Track.hh"
#include "BDSDebug.hh"

// basic inheritance - just use everything normally from G4UserLimits but 
// replace one function in inherited class
// default values are defined in G4UserLimits so all particles continue
BDSTerminatorUserLimits::BDSTerminatorUserLimits(G4double ustepMax,
						 G4double utrakMax,
						 G4double utimeMax,
						 G4double uekinMin,
						 G4double urangMin):
  G4UserLimits(ustepMax,
		 utrakMax,
		 utimeMax,
		 uekinMin,
		 urangMin),
  keeprunningEK(0.0),
  stoprunningEK(DBL_MAX)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  verbose     = BDSExecOptions::Instance()->GetVerbose();
  turnsToTake = BDSGlobalConstants::Instance()->GetTurnsToTake();
}

BDSTerminatorUserLimits::BDSTerminatorUserLimits(const G4String& type,
						 G4double ustepMax,
						 G4double utrakMax,
						 G4double utimeMax,
						 G4double uekinMin,
						 G4double urangMin):
  G4UserLimits(type,
	       ustepMax,
	       utrakMax,
	       utimeMax,
	       uekinMin,
	       urangMin),
  keeprunningEK(0.0),
  stoprunningEK(DBL_MAX)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  verbose     = BDSExecOptions::Instance()->GetVerbose();
  turnsToTake = BDSGlobalConstants::Instance()->GetTurnsToTake();
}

inline G4double BDSTerminatorUserLimits::GetUserMinEkine(const G4Track& /*trk*/)
{
  // does the number of turns passed == number of turns to take
  G4int turnsTaken = BDSGlobalConstants::Instance()->GetTurnsTaken();
#ifdef BDSDEBUG
  // for some reason the __METHOD_NAME__ can't identify this function so hard coded its name
  G4cout << "BDSTerminatorUserLimits::GetUserMinEkine> turns taken : " << turnsTaken << G4endl;
#endif
  if (turnsTaken >= turnsToTake)
    {
#ifdef BDSDEBUG
      G4cout << "Requested number of turns completed - stopping all particles" << G4endl;
#endif
      return stoprunningEK;
    } // yes - stop: return DBL_MAX eV so no particles will be tracked
  else
    {
      return keeprunningEK;
    } // no  - keep going: return 0 eV so all are tracked
}


