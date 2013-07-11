// Provide interface to output


#ifndef BDSOutput_h
#define BDSOutput_h 

#include "BDSGlobalConstants.hh"
#include "BDSSampler.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerSD.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSEnergyCounterHit.hh"

#include "BDSLWCalorimeter.hh"
#include "BDSLWCalorimeterHit.hh"

#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

#include <fstream>
#include <vector>
// is the root output supported?
#ifdef USE_ROOT
#include "TROOT.h"
#include "TH1F.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TTree.h"
#include "TH3F.h"
#endif

enum BDSOutputFormat {
  _ASCII = 0,
  _ROOT = 1
  //, _ASCII_ROOT = 2
};

class BDSOutput {

public: 
  BDSOutput(); // default constructor
  BDSOutput(BDSOutputFormat format);

  void SetFormat(BDSOutputFormat format);
  void Init(G4int FileNum);
  ~BDSOutput();

  void WriteHits(BDSSamplerHitsCollection*);
  void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  G4int WriteTrajectory(TrajectoryVector* TrajVec);
  G4int WriteTrajectory(std::vector<G4VTrajectory*> TrajVec);

  void Echo(G4String str);

  G4int Commit(); //G4int FileNum);   // close the event
  void Write();           // close the event
  // for root output

#ifdef USE_ROOT
  TFile* theRootOutputFile;
  //  TTree *theLWCalorimeterTree;

  TH1F *EnergyLossHisto;
  //  TH3F *EnergyLossHisto3d;
  TTree *PrecisionRegionEnergyLossTree;
  TTree *EnergyLossTree;
#endif

  G4int nSamplers;
  G4double zMax, transMax; //Maximum values of longitudinal and transverse global position
  //BDSSamplerSD* BDSSamplerSensDet;
  std::vector <G4String> SampName;
  std::vector <G4String> CSampName;
private:
  G4String _filename;
  G4int format;
  std::ofstream of;
  std::ofstream ofEloss;
  int outputFileNumber;

//#ifdef USE_ROOT
  float x0,xp0,y0,yp0,z0,zp0,E0,t0;
  float x,xp,y,yp,z,zp,E,Edep,t;
  float X,Xp,Y,Yp,Z,Zp,s,weight,EWeightZ;
  int part,nev, pID, theID, track_id;
  float z_el,E_el;
  float x_el_p,y_el_p,z_el_p,E_el_p;
  int part_el_p,pID_el_p, weight_el_p;
  char volumeName_el_p[100];
//#endif

};

extern BDSOutput* bdsOutput;
#endif
