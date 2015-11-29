#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#ifdef USE_ROOT

#include "BDSOutputROOTBase.hh"
#include "BDSTrajectory.hh"

#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/**
 * @brief Lightweight ROOT output class
 * 
 * This contains the basic ROOT output information.
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSOutputROOT: public BDSOutputROOTBase
{
public:
  /// Default constructor
  BDSOutputROOT(); 
  virtual ~BDSOutputROOT();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*) override;
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*) override;
  /// write primary loss histo
  virtual void WritePrimaryLoss(BDSEnergyCounterHit*) override;
  /// write primary hits histo
  virtual void WritePrimaryHit(BDSEnergyCounterHit*) override;
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSTunnelHitsCollection*) override;
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec) override;
  /// write primary hit
  virtual void WritePrimary(G4double totalEnergy,
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
			    G4int    turnsTaken) override;

  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn) override;
  virtual void FillEvent() {};
  virtual void Commit() override;  ///< close the file
  virtual void Write()  override;  ///< close and open new file

private:

  void Init(); ///< output initialisation

  //  TTree* BuildSamplerTree(G4String name); ///< build sampler TTree
  TFile* theRootOutputFile;
  
  TTree* PrecisionRegionEnergyLossTree;
  TTree* EnergyLossTree;
  TTree* PrimaryLossTree;
  TTree* PrimaryHitsTree;
  TTree* TunnelLossTree;

  TH2F*  tunnelHitsHisto;

  /// Build sampler TTree
  virtual TTree* BuildSamplerTree(G4String name);
};

#endif // USE_ROOT
#endif
