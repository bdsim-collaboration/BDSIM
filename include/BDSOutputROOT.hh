#ifndef BDSOutputROOT_h
#define BDSOutputROOT_h 

#ifdef USE_ROOT

#include "BDSOutputBase.hh"
#include "BDSTrajectory.hh"

#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/**
 * @brief ROOT output class
 * 
 * Write BDSIM output to ROOT files. Originally
 * part of BDSIM code base and recently developed and maintained
 * by Jochem Snuverink, Lawrence Deacon & Laurie Nevay
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSOutputROOT: public BDSOutputBase
{
public: 

  BDSOutputROOT(); // default constructor
  //  BDSOutput(BDSOutputFormat format);
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

  virtual void WriteTrackerBunch(G4String samplerName, TRKBunch* bunch, bool primary=false);
  virtual void WriteTrackerPrimaryLoss(TRKBunch* lostBunch, int turnsTaken);


  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn) override;
  virtual void Commit() override;  ///< close the file
  virtual void Write()  override;  ///< close and open new file

private:

  void Init(); ///< output initialisation

  TTree* BuildSamplerTree(G4String name); ///< build sampler TTree
  TFile* theRootOutputFile;
  
  TTree* PrecisionRegionEnergyLossTree;
  TTree* EnergyLossTree;
  TTree* PrimaryLossTree;
  TTree* PrimaryHitsTree;
  TTree* TunnelLossTree;

  TH2F*  tunnelHitsHisto;

  /// vector of Sampler trees
  std::vector<TTree*> samplerTrees;
  
  // members for writing to TTrees
  float x0=0.0,xp0=0.0,y0=0.0,yp0=0.0,z0=0.0,zp0=0.0,E0=0.0,t0=0.0;
  float x_prod=0.0,xp_prod=0.0,y_prod=0.0,yp_prod=0.0,z_prod=0.0,zp_prod=0.0,E_prod=0.0,t_prod=0.0;
  float x_lastScat=0.0,xp_lastScat=0.0,y_lastScat=0.0,yp_lastScat=0.0,z_lastScat=0.0,zp_lastScat=0.0,E_lastScat=0.0,t_lastScat=0.0;
  std::string process;

  /// local parameters
  float x=0.0,xp=0.0,y=0.0,yp=0.0,z=0.0,zp=0.0,E=0.0,t=0.0;
  /// global parameters
  float X=0.0,Xp=0.0,Y=0.0,Yp=0.0,Z=0.0,Zp=0.0,S=0.0,weight=0.0,stepLength=0.0;
  /// PDG id, event number, parentID, trackID, turn number
  int part=-1,eventno=-1, pID=-1, track_id=-1, turnnumber=-1;
  
  /// tunnel hits variables
  float E_tun=0.0, S_tun=0.0, r_tun=0.0, angle_tun=0.0;

  /// volume name
  char volumeName[100];

  /// fill members so that trees can be written
  void FillHit(BDSEnergyCounterHit* hit);

  /// write hit to TTree
  void WriteRootHit(TTree*   Tree,
		    G4double InitTotalenergy, 
		    G4double InitX, 
		    G4double InitY, 
		    G4double InitZ, 
		    G4double InitXPrime, 
		    G4double InitYPrime, 
		    G4double InitZPrime, 
		    G4double InitT, 
		    G4double ProdTotalenergy, 
		    G4double ProdX, 
		    G4double ProdY, 
		    G4double ProdZ, 
		    G4double ProdXPrime, 
		    G4double ProdYPrime, 
		    G4double ProdZPrime, 
		    G4double ProdT, 
		    G4double LastScatTotalenergy, 
		    G4double LastScatX, 
		    G4double LastScatY, 
		    G4double LastScatZ, 
		    G4double LastScatXPrime, 
		    G4double LastScatYPrime, 
		    G4double LastScatZPrime, 
		    G4double LastScatT, 
		    G4double Totalenergy, 
		    G4double X, 
		    G4double Y, 
		    G4double Z, 
		    G4double XPrime, 
		    G4double YPrime, 
		    G4double ZPrime, 
		    G4double T, 
		    G4double GlobalX, 
		    G4double GlobalY, 
		    G4double GlobalZ, 
		    G4double GlobalXPrime, 
		    G4double GlobalYPrime, 
		    G4double GlobalZPrime, 
		    G4double S, 
		    G4double Weight, 
		    G4int    PDGtype, 
		    G4int    EventNo, 
		    G4int    ParentID,
		    G4int    TrackID, 
		    G4int    TurnsTaken,
		    G4String Process);
  
};

extern BDSOutputBase* bdsOutput;

#endif // USE_ROOT
#endif
