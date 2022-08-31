/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSRUNACTION_H
#define BDSRUNACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserRunAction.hh"

#include <chrono>
#include <ctime>
#include <string>

class BDSBunch;
class BDSEventAction;
class BDSEventInfo;
class BDSOutput;
class G4Run;

/**
 * @brief Control over the beginning and end of run actions.
 *
 * Unlike the regular Geant4 run action we call a beginning of run
 * action on the bunch distribution (when we know the number of events
 * to run).
 */

class BDSRunAction: public G4UserRunAction
{
public:
  BDSRunAction(BDSOutput*      outputIn,
	       BDSBunch*       bunchGeneratorIn,
	       G4bool          usingIonsIn,
	       BDSEventAction* eventActionIn,
	       G4String        trajectorySamplerIDIn);
  virtual ~BDSRunAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  void ParticleCount(G4String, G4double, G4double);

private:
  BDSRunAction() = delete;
  
  /// Iterate over all particles and print out all process names for each. This
  /// is private as using at the wrong time will result in Geant4 crashing.
  void PrintAllProcessesForAllParticles() const;

  /// Access the sampler registry and find the integer sampler IDs for any
  /// samplers named in the trajectorySamplerID string. Do this at the beginning
  /// of a run and update event action because it should only happen once and
  /// it depends on the construction of the geometry being complete.
  void SetTrajectorySamplerIDs() const;

  /// Check whether various trajectory options that are geometry dependent make
  /// sense and warn if not. Done now because geometry is built before run.
  void CheckTrajectoryOptions() const;

  struct ParticleData {
        ParticleData()
                : fCount(0), fEmean(0.), fEmin(0.), fEmax(0.), fTmean(-1.) {}
        ParticleData(G4int count, G4double ekin, G4double emin, G4double emax,
                     G4double meanLife)
                : fCount(count), fEmean(ekin), fEmin(emin), fEmax(emax),
                  fTmean(meanLife) {}
        G4int     fCount;
        G4double  fEmean;
        G4double  fEmin;
        G4double  fEmax;
        G4double  fTmean;
  };
  
  BDSOutput*    output;           ///< Cache of output instance. Not owned by this class.
  time_t        starttime;
  std::string   seedStateAtStart; ///< Seed state at start of the run.
  BDSEventInfo* info;
  BDSBunch*     bunchGenerator;   ///< Cache of bunch generator.
  const G4bool  usingIons;        ///< Cache of whether ions are being used (for particle table writing out).
  std::clock_t  cpuStartTime;     ///< Start time of run.
  BDSEventAction* eventAction;    ///< Event action for updating information at start of run.
  G4String        trajectorySamplerID; ///< Copy of option.

  std::map<G4String,ParticleData>  fParticleDataMap;
  const G4Run* run;

};

#endif

