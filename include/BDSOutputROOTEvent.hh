#ifndef BDSOutputROOTEvent_h
#define BDSOutputROOTEvent_h

#ifdef USE_ROOT 

#include "globals.hh"

#include "BDSOutputBase.hh"

#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/**
 * @brief ROOT Event output class
 * 
 * Write BDSIM output to ROOT files. Originally
 * part of BDSIM code base and recently developed and maintained
 * by Jochem Snuverink, Lawrence Deacon & Laurie Nevay
 * 
 * @author Stewart Boogert <stewart.boogert@rhul.ac.uk>
 */

class BDSOutputROOTEvent : public BDSOutputBase 
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
  
  /// write and close and open new file
  virtual void Commit();
  
  /// write and close the file
  virtual void Write();
  
private:

  void Init();

  TFile *theRootOutputFile;
  // create primary structures 
  // create sampler structures 
  // create energy hit structures 
};




#endif

#endif
