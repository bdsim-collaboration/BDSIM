#ifndef BDSRunAction_h
#define BDSRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <ctime>

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

  //indices of histograms with analysis manager
  G4int phitsindex;
  G4int plossindex;
  G4int elossindex;
  G4int phitspeindex;
  G4int plosspeindex;
  G4int elosspeindex;
};

#endif

