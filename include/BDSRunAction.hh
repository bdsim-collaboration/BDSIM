#ifndef BDSRUNACTION_H
#define BDSRUNACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserRunAction.hh"

#include <ctime>
#include <string>

class G4Run;

/**
 * @brief Control over the beginning and end of run actions.
 */

class BDSRunAction: public G4UserRunAction
{
public:
  BDSRunAction();
  virtual ~BDSRunAction();
  
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:
  time_t starttime;
  time_t stoptime;
  std::string seedStateAtStart; ///< Seed state at start of the run.
};

#endif

