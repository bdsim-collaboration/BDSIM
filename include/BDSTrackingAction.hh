#ifndef BDSTRACKINGACTION_H
#define BDSTRACKINGACTION_H

#include "G4Track.hh"
#include "G4UserTrackingAction.hh"

class BDSTrackingAction: public G4UserTrackingAction
{
public:
  BDSTrackingAction();
  virtual ~BDSTrackingAction(){;}
   
  virtual void PreUserTrackingAction(const G4Track*);
};

#endif
