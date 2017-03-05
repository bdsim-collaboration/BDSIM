#ifndef BDSSTEPPINGACTION_H
#define BDSSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"

/**
 * @brief Provide extra output for Geant4 through a verbose stepping action.
 */

class BDSSteppingAction: public G4UserSteppingAction
{
public:
  BDSSteppingAction();
  BDSSteppingAction(G4bool verboseStep,
		    G4int  verboseEventNumber = -1);
  virtual ~BDSSteppingAction();

  virtual void UserSteppingAction(const G4Step* step);

private:
  void VerboseSteppingAction(const G4Step* step);

  const G4bool verboseStep; ///< Cache of whether to print out verbose step information
  const G4int  verboseEventNumber; ///< Event number of desired increased verbosity.
};

#endif
