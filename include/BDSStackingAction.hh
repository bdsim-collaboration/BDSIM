/*
  Stacking Action - action performed before particles pushed to the stack

*/


#ifndef __BDSSTACKING_ACTION__
#define __BDSSTACKING_ACTION__

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
 };

#endif

