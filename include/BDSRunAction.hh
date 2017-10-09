#ifndef BDSRUNACTION_H
#define BDSRUNACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserRunAction.hh"

#include <ctime>
#include <string>

class BDSEventInfo;
class BDSOutput;
class G4Run;

/**
 * @brief Control over the beginning and end of run actions.
 */

class BDSRunAction: public G4UserRunAction
{
public:
  explicit BDSRunAction(BDSOutput* outputIn);
  virtual ~BDSRunAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  BDSOutput*    output;         ///< Cache of output instance. Not owned by this class.
  time_t        starttime;
  std::string   seedStateAtStart; ///< Seed state at start of the run.
  BDSEventInfo* info;
};

#endif

