#ifndef BDSRUNACTION_H
#define BDSRUNACTION_H

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <ctime>

class G4Run;

/**
 * @brief Control over the beginning and end or run actions.
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

  //indices of histograms with analysis manager
  G4int phitsindex;
  G4int plossindex;
  G4int elossindex;
  G4int phitspeindex;
  G4int plosspeindex;
  G4int elosspeindex;
};

#endif

