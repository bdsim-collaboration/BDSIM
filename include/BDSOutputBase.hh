/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#ifndef BDSOutputBase_h
#define BDSOutputBase_h 

#include "globals.hh"

#include <ctime>
#include <ostream>
#include <set>
#include <string>
#include <vector>

// forward declarations
template <class T> class G4THitsCollection;
class BDSEnergyCounterHit;
typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
class BDSHistogram1D;
class BDSOutputROOTEventInfo;
class BDSSamplerHit;
typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
class BDSTrajectory;
class BDSTrajectoryPoint;
typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;

class TRKBunch;

/**
 * @brief Output base class that defines interface for all output types.
 */

class BDSOutputBase
{
public: 
  BDSOutputBase(); // default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputBase() {};

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*) = 0;
  
  /// write energy deposition hits
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*) = 0;
  
  /// write where primaries stop being primaries
  virtual void WritePrimaryLoss(BDSTrajectoryPoint* ploss) = 0;

  /// write where primaries impact
  virtual void WritePrimaryHit(BDSTrajectoryPoint* phits) = 0;

  /// write tunnel hits
  virtual void WriteTunnelHits(BDSEnergyCounterHitsCollection* tunnelHits) = 0;
  
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec) = 0;
  
  /// write primary hit
  virtual void WritePrimary(G4double E,
                            G4double x0,
                            G4double y0,
                            G4double z0,
                            G4double xp,
                            G4double yp,
                            G4double zp,
                            G4double t,
                            G4double weight,
                            G4int    PDGType,
                            G4int    nEvent,
                            G4int    TurnsTaken) = 0;

  /// Write additional information about event such as timing.
  virtual void WriteEventInfo(const time_t&  startTime,
			      const time_t&  stopTime,
			      const G4float& duration,
                              const std::string &seedStateAtStart) = 0;

  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* info) = 0;

  /// write a histgoram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn) = 0;

  /// write tracker bunch
  virtual void WriteTrackerBunch(G4String samplerName, TRKBunch* bunch, bool primary=false) = 0;
  virtual void WriteTrackerPrimaryLoss(TRKBunch* lostBunch, int turnsTaken) = 0;
  
  /// write a complete event
  virtual void FillEvent() = 0;

  /// open new file
  virtual void Initialise()=0;
  
  /// write the data to file
  virtual void Write(const time_t&  startTime,
                     const time_t&  stopTime,
                     const G4float& duration,
                     const std::string& seedStateAtStart) = 0;

  /// close file
  virtual void Close() = 0;

  /// write, close and open new file
  void Commit(const time_t&  startTime,
              const time_t&  stopTime,
              const G4float& duration,
              const std::string& seedStateAtStart);

  /// Invalid names for samplers - kept here as this is where the output structures are created.
  const static std::set<G4String> protectedNames;
  
  /// Test whether a sampler name is invalid or not.
  static bool InvalidSamplerName(const G4String& samplerName);
  
  /// Feedback for protected names
  static void PrintProtectedNames(std::ostream& out);
  
protected:
  /// current event number
  int eventNumber;
  /// number of events stored per file
  int numberEventPerFile;
  ///number of output file
  int outputFileNumber;
  /// file name
  G4String filename;
};

extern BDSOutputBase* bdsOutput;
#endif
