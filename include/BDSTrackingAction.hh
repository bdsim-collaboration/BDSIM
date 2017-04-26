#ifndef BDSTRACKINGACTION_H
#define BDSTRACKINGACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserTrackingAction.hh"

class G4Track;

/**
 * @brief Action to decide whether or not to store trajectory information.
 */

class BDSTrackingAction: public G4UserTrackingAction
{
public:
  BDSTrackingAction();
  explicit BDSTrackingAction(G4bool batchMode);
  
  virtual ~BDSTrackingAction(){;}

  /// Used to decide whether or not to store trajectories.
  virtual void PreUserTrackingAction(const G4Track* track);

private:
  /// Whether we're using the visualiser - in which case always store
  /// trajectories for visualisation.
  G4bool interactive;

  /// Flag from global constants to control storing all trajectories. Cache
  /// this from global constants at the beginning as it doesn't change.
  G4bool storeTrajectory;
};

#endif
