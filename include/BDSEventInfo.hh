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
 * This is basically a wrapper for BDSOutputROOTEventInfo. Can't inherit
 * BDSOutputROOTEventInfo as the Print overload for geant4 clashes with
 * one from TObject.  Keep an instance and use that.
 * 
 * @author Laurie Nevay
 */

class BDSEventInfo: public G4VUserEventInformation
{
public:
  BDSEventInfo();
  virtual ~BDSEventInfo();

  virtual void Print() const;

  /// @{ Setters
  inline void SetStartTime(time_t startTimeIn) {info->startTime = startTimeIn;}
  inline void SetStopTime(time_t stopTimeIn)   {info->stopTime  = stopTimeIn;}
  inline void SetDuration(G4float durationIn)  {info->duration  = (float)durationIn;}
  inline void SetSeedStateAtStart(G4String seedStateAtStartIn) {info->seedStateAtStart = (std::string)seedStateAtStartIn;}
  inline void SetIndex(G4int indexIn)          {info->index     = (int)indexIn;}
  /// @}

  /// Accessor.
  inline const BDSOutputROOTEventInfo* GetInfo() const {return info;}

  inline void Flush() {info->Flush();}

private:
  /// Instance of info.
  BDSOutputROOTEventInfo* info;
};

#endif
