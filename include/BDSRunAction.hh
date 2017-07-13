#ifndef BDSRUNACTION_H
#define BDSRUNACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserRunAction.hh"

#include <ctime>
#include <string>

class BDSOutput;
class G4Run;

/**
 * @brief Control over the beginning and end of run actions.
 */

class BDSRunAction: public G4UserRunAction
{
public:
  BDSRunAction(BDSOutput* outputIn);
  virtual ~BDSRunAction();
  
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:
  BDSOutput* output;         ///< Cache of output instance. Not owned by this class.
  time_t starttime;
  time_t stoptime;
  std::string seedStateAtStart; ///< Seed state at start of the run.
};

#endif

