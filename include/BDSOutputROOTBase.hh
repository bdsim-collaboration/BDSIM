#ifndef BDSOUTPUTROOTBASE_H
#define BDSOUTPUTROOTBASE_H

#include "BDSOutputBase.hh"
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

class BDSOutputROOTBase: public BDSOutputBase
{
public:
  /// Default constructor
  BDSOutputROOTBase(); 
  virtual ~BDSOutputROOTBase();

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
  virtual void Commit() override;  ///< close the file
  virtual void Write()  override;  ///< close and open new file

protected:
  /// Output initialisation
  void Init(); 
  
  /// Build sampler TTree
  virtual TTree* BuildSamplerTree(G4String name) = 0;

  virtual TTree* BuildSamplerTreeBasic(G4String name);
  
  /// Write a root hit to file based on the sampler hit.
  virtual void WriteRootHit(TTree*         tree,
			    BDSSamplerHit* hit,
			    G4bool         fillTree = true);

  
  /// Write hit to TTree
  void WriteRootHit(TTree*   Tree,
		    G4double totalEnergy,
		    G4double x,
		    G4double y,
		    G4double z,
		    G4double xPrime,
		    G4double yPrime,
		    G4double zPrime,
		    G4double T,
		    G4double S,
		    G4double Weight,
		    G4int    PDGtype,
		    G4int    EventNo,
		    G4int    ParentID,
		    G4int    TrackID,
		    G4int    TurnsTaken,
		    G4String Process,
		    G4bool   fillTree = true);
  
  /// Fill members so that trees can be written
  void FillHit(BDSEnergyCounterHit* hit);

  /// Members for writing to TTrees
  /// Local parameters
  float x=0.0,xp=0.0,y=0.0,yp=0.0,z=0.0,zp=0.0,E=0.0,t=0.0,S=0.0;
  /// Global parameters
  float X=0.0,Y=0.0,Z=0.0,T=0.0;
  /// PDG id, event number, parentID, trackID, turn number
  int part=-1,eventno=-1, pID=-1, track_id=-1, turnnumber=-1,weight=0.0,stepLength=0.0;
  std::string process;
  
  /// tunnel hits variables
  float E_tun=0.0, S_tun=0.0, r_tun=0.0, angle_tun=0.0;

  /// vector of Sampler trees
  std::vector<TTree*> samplerTrees;

private:
  
  TFile* theRootOutputFile;
  
  TTree* PrecisionRegionEnergyLossTree;
  TTree* EnergyLossTree;
  TTree* PrimaryLossTree;
  TTree* PrimaryHitsTree;
  TTree* TunnelLossTree;

  TH2F*  tunnelHitsHisto;

  /// volume name
  char volumeName[100];
};

extern BDSOutputBase* bdsOutput;

#endif
