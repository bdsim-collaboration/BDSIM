/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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
