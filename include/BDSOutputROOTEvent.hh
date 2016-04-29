#ifndef BDSOUTPUTROOTEVENT_H
#define BDSOUTPUTROOTEVENT_H

#include "globals.hh"

#include <ctime>
#include <map>

#include "BDSOutputBase.hh"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"


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
  virtual void WritePrimaryLoss(BDSEnergyCounterHit* ploss);

  /// write where primaries impact
  virtual void WritePrimaryHit(BDSEnergyCounterHit* phits);

  /// write tunnel hits
  virtual void WriteTunnelHits(BDSTunnelHitsCollection* tunnelHits);
  
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

  /// fill event structure
  virtual void FillEvent();
  
  /// write event info
  virtual void WriteEventInfo(time_t startTime, time_t stopTime, G4float duration);

  virtual void Initialise(); ///< open the file
  virtual void Write();      ///< write to file
  virtual void Close();      ///< close the file

  /// clear structures
  void Flush();

  virtual BDSOutputROOTEventHistograms* GetEventAnalysis();
  virtual BDSOutputROOTEventHistograms* GetRunAnalysis();




private:

  // output file
  TFile *theRootOutputFile = nullptr;

  // options structure
  // BDSOutputROOTEventOptions *theOptionsOutput;

  // options tree
  TTree *theOptionsOutputTree = nullptr;

  // model tree 
  TTree *theModelOutputTree = nullptr;

  // output tree
  TTree *theRootOutputTree = nullptr;

  // output histogram tree
  TTree *theRunOutputTree  = nullptr;

  // primary structure 
#ifndef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<float> *primary = nullptr;
#else 
  BDSOutputROOTEventSampler<double> *primary = nullptr;
#endif
  // sampler structures 
  std::map<G4String, BDSOutputROOTEventSampler<float>*> samplerMap; // will remove
#ifndef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<float>*> samplerTrees;
#endif

  // energy loss
  BDSOutputROOTEventLoss        *eLoss       = nullptr;
  // primary first hit
  BDSOutputROOTEventLoss        *pFirstHit   = nullptr;
  // primary final hit
  BDSOutputROOTEventLoss        *pLastHit    = nullptr;
  // tunnel hit
  BDSOutputROOTEventLoss        *tHit        = nullptr;
  // trajectory
  BDSOutputROOTEventTrajectory  *traj        = nullptr;
  // run histograms
  BDSOutputROOTEventHistograms  *runHistos   = nullptr;
  // event histograms
  BDSOutputROOTEventHistograms  *evtHistos   = nullptr;
  // event information
  BDSOutputROOTEventInfo         *evtInfo     = nullptr;
};


#endif
