#ifndef BDSUSERTRACKINGACTION_H
#define BDSUSERTRACKINGACTION_H

#include "G4UserTrackingAction.hh"


class BDSUserTrackingAction: public G4UserTrackingAction
{
public:
  BDSUserTrackingAction(){;};
  virtual ~BDSUserTrackingAction();
   
  virtual void PreUserTrackingAction(const G4Track*);

};

#endif
