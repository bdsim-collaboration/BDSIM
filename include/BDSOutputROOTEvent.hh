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
#ifndef BDSOUTPUTROOTEVENT_H
#define BDSOUTPUTROOTEVENT_H

#include "globals.hh"

#include <ctime>
#include <map>
#include <string>
#include <vector>

#include "BDSOutputBase.hh"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventRunInfo.hh"


#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/**
 * @brief ROOT Event output class.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEvent: public BDSOutputBase 
{
public:
  BDSOutputROOTEvent();
  virtual ~BDSOutputROOTEvent();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*);
  
  /// write energy deposition hits
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  
  /// write where primaries stop being primaries
  virtual void WritePrimaryLoss(BDSTrajectoryPoint* ploss);

  /// write where primaries impact
  virtual void WritePrimaryHit(BDSTrajectoryPoint* phits);

  /// write tunnel hits
  virtual void WriteTunnelHits(BDSEnergyCounterHitsCollection* tunnelHits);
  
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec);
  
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
			    G4int    TurnsTaken);

  /// write a histgoram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn);

  /// write tracker bunch
  virtual void WriteTrackerBunch(G4String /*samplerName*/, TRKBunch* /*bunch*/, bool /*primary=false*/) {};
  virtual void WriteTrackerPrimaryLoss(TRKBunch* /*lostBunch*/, int /*turnsTaken*/) {};
  /// fill event structure
  virtual void FillEvent();
  
  /// write event info
  virtual void WriteEventInfo(const time_t &startTime, const time_t &stopTime, const G4float &duration,
                              const std::string &seedStateAtStart);
  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* info);

  virtual void Initialise(); ///< open the file
  virtual void Write(const time_t &startTime, const time_t &stopTime, const G4float &duration,
                       const std::string &seedStateAtStart);      ///< write to file
  virtual void Close();      ///< close the file

  /// clear structures
  void Flush();

private:

  /// create histograms
  void CreateHistograms();
  
  /// output file
  TFile *theRootOutputFile = nullptr;

  /// options structure
  // BDSOutputROOTEventOptions *theOptionsOutput;

  /// options tree
  TTree *theOptionsOutputTree = nullptr;

  /// model tree
  TTree *theModelOutputTree = nullptr;

  /// event tree
  TTree *theEventOutputTree = nullptr;

  /// output histogram tree
  TTree *theRunOutputTree  = nullptr;

  /// primary structure
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double> *primary = nullptr;
#else 
  BDSOutputROOTEventSampler<float> *primary = nullptr;
#endif
  /// sampler structures
#ifdef __ROOTDOUBLE__
  //  std::map<G4String, BDSOutputROOTEventSampler<double>*> samplerMap; // will remove
  std::vector<BDSOutputROOTEventSampler<double>*> samplerTrees;
#else
  std::vector<BDSOutputROOTEventSampler<float>*> samplerTrees;
#endif
  std::vector<std::string> samplerNames;

  /// run information
  BDSOutputROOTEventRunInfo     *runInfo     = nullptr;
  /// run histograms
  BDSOutputROOTEventHistograms  *runHistos   = nullptr;


  /// energy loss
  BDSOutputROOTEventLoss        *eLoss       = nullptr;
  /// primary first hit
  BDSOutputROOTEventLoss        *pFirstHit   = nullptr;
  /// primary final hit
  BDSOutputROOTEventLoss        *pLastHit    = nullptr;
  /// tunnel hit
  BDSOutputROOTEventLoss        *tHit        = nullptr;
  /// trajectory
  BDSOutputROOTEventTrajectory  *traj        = nullptr;
  /// event histograms
  BDSOutputROOTEventHistograms  *evtHistos   = nullptr;
  /// event information
  BDSOutputROOTEventInfo        *evtInfo     = nullptr;

  G4bool useScoringMap  = false;
  G4bool writePrimaries = true;
};


#endif
