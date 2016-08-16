#ifndef BDSEVENTINFO_H
#define BDSEVENTINFO_H

#include "BDSOutputROOTEventInfo.hh"

#include "globals.hh" // geant4 types / globals
#include "G4VUserEventInformation.hh"

/**
 * @brief Interface to store event information use G4 hooks.
 * 
 * This class inherits G4VUserEventInformation so it can be used
 * to pass information around as part of a G4Event (required by G4).
 * This is basically a wrapper for BDSOutputROOTEventInfo. Although
 * the inheritance of BDSOutputROOTEventInfo provides public access
 * to its members, setters are provided here for convenience and to
 * make the interface more explicit / cleaner.
 * 
 * @author Laurie Nevay
 */

class BDSEventInfo: public G4VUserEventInformation, public BDSOutputROOTEventInfo
{
public:
  BDSEventInfo();
  virtual ~BDSEventInfo();

  virtual void Print() const;

  /// @{ Setters
  inline void SetStartTime(time_t startTimeIn) {startTime = startTimeIn;}
  inline void SetStopTime(time_t stopTimeIn)   {stopTime  = stopTimeIn;}
  inline void SetDuration(G4double durationIn) {duration  = durationIn;}
  inline void SetSeedStateAtStart(G4String seedStateAtStartIn) {seedStateAtStart = seedStateAtStartIn;}
  /// @}
};

#endif
