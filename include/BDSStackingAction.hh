#ifndef BDSSTACKINGACTION_H
#define BDSSTACKINGACTION_H

#include "globals.hh"
#include "G4UserStackingAction.hh"

class G4Track;

class BDSStackingAction : public G4UserStackingAction
{
  public:
    BDSStackingAction();
    virtual ~BDSStackingAction();

  public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void NewStage();
  virtual void PrepareNewEvent();
  void countPhoton(const G4Track* aTrack);  
 };

#endif

