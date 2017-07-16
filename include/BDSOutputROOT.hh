#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#include "globals.hh"

#include <ctime>
#include <map>
#include <string>
#include <vector>

#include "BDSOutput.hh"

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

class BDSOutputROOT: public BDSOutput 
{
public:
  /// Constructor with default file name (without extension or number suffix).
  /// Also, file number offset to start counting suffix from.
  BDSOutputROOT(G4String fileName,
		G4int    fileNumberOffset);
  virtual ~BDSOutputROOT();

  virtual void NewFile();                     ///< Open a new file.
  virtual void InitialiseGeometryDependent(); ///< Build samplers.
  virtual void CloseFile();                   ///< Write contents and close file.

  /// Fill the local structure with primary vertex information.
  virtual void FillPrimary(G4double E,
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
			   G4int    turnsTaken);

  /// Fill event summary information.
  virtual void FillEventInfo(const BDSEventInfo *info);

  /// Fill sampler hits into output structures.
  virtual void FillSamplerHits(const BDSSamplerHitsCollection *hits, const HitsType);

  /// Fill a collection of energy hits into the appropriate output structure.
  virtual void FillEnergyLoss(const BDSEnergyCounterHitsCollection *, const LossType);

  /// Fill the hit where the primary particle impact.
  virtual void FillPrimaryHit(const BDSTrajectoryPoint *phit);
  
  /// Fill the hit where the primary stopped being a primary.
  virtual void FillPrimaryLoss(const BDSTrajectoryPoint *ploss);

  /// Copy a set of trajectories to the output structure.
  virtual void FillTrajectories(const std::vector<BDSTrajectory *> &TrajVec);

  /// Fill run level summary information.
  virtual void FillRunInfo(const BDSEventInfo *info);

  /// Copy from local event structures to the actual file.  Ony event level
  /// structures are copied.
  virtual void WriteFileEventLevel();

  /// Clear the local structures in this class in preparation for a new event.
  virtual void ClearStructuresEventLevel();

  /// Copy from local run structures to the actual file.  Only run level
  /// structures are copied.
  virtual void WriteFileRunLevel();

  /// Clear the local structures in this class in preparation for a new run.
  virtual void ClearStructuresRunLevel();
  
  /// Clear structures.
  void Flush();

private:
  /// No default constructor.
  BDSOutputROOT() = delete;

  /// Create histograms.
  void CreateHistograms();
  
  ///@{ create histograms for evtHistos and runHistos
  void Create1DHistogram(G4String name, G4String title,
                         G4int nbins, G4double xmin, G4double xmax);
  void Create1DHistogram(G4String name, G4String title,
                         std::vector<double>& edges);
  ///@}

  /// Calculate the number of bins and required maximum s.
  void CalculateHistogramParameters();

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

  /// Wether we've set up the member vector of samplers. Can only be done once the geometry
  /// is constructed so it's done in initialisation. This happens with each new file, but we
  /// should only prepare the local samplers once, hence this cache variable.
  G4bool localSamplersInitialised;
  
  G4bool useScoringMap  = false;

  /// The maximum s in mm such that there is an integer number of
  /// elossHistoBinWidths along the line. Used for histogramming purposes.
  G4double sMaxHistograms;

  G4int nbins; ///< Number of bins for each histogram required.
};


#endif
