// Provide interface to output


#ifndef BDSOutput_h
#define BDSOutput_h 

#include "BDSEnergyCounterHit.hh"
#include "BDSOutputFormat.hh"
#include "BDSSamplerHit.hh"

#include "G4Trajectory.hh"

#include <fstream>
#include <vector>
// is the root output supported?
#ifdef USE_ROOT
#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
//#include "TH3F.h"
#endif

typedef BDSTypeSafeEnum<BDSOutputFormatDef,int> BDSOutputFormat;

class BDSOutput {

public: 

  BDSOutput(BDSOutputFormat format);

  ~BDSOutput();
  
  void WriteHits(BDSSamplerHitsCollection*);
  void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  G4int WriteTrajectory(std::vector<G4VTrajectory*> &TrajVec);

  G4int Commit(); //G4int FileNum);   // close the event
  void Write();           // close the event
  void WritePrimary(G4String, G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4int, G4int, G4int);

private:
  BDSOutput();  // default constructor, not used
  void SetFormat(BDSOutputFormat format);
  void Init();

  //G4double zMax, transMax; //Maximum values of longitudinal and transverse global position
  //std::vector <G4String> SampName;
  //std::vector <G4String> CSampName;

  G4String _filename;
  BDSOutputFormat format;
  std::ofstream of;
  std::ofstream ofEloss;
  //number of output file
  int outputFileNumber;

#ifdef USE_ROOT
    // for root output
  void BuildSamplerTree(G4String name);
  TFile* theRootOutputFile;
  //  TTree *theLWCalorimeterTree;

  TH1F *EnergyLossHisto;
  //  TH3F *EnergyLossHisto3d;
  TTree *PrecisionRegionEnergyLossTree;
  TTree *EnergyLossTree;
  
  float x0,xp0,y0,yp0,z0,zp0,E0,t0;
  float x,xp,y,yp,z,zp,E,t; //Edep;
  float X,Xp,Y,Yp,Z,Zp,s,weight; //,EWeightZ;
  int part,nev, pID, track_id, turnnumber;
  float z_el,E_el;
  float x_el_p,y_el_p,z_el_p,E_el_p;
  int part_el_p, weight_el_p;
  char volumeName_el_p[100];

  void WriteRootHit(G4String Name, G4double InitMom, G4double InitX, G4double InitY, G4double InitZ, G4double InitXPrime, G4double InitYPrime, G4double InitZPrime, G4double InitT, G4double Mom, G4double X, G4double Y, G4double Z, G4double XPrime, G4double YPrime, G4double ZPrime, G4double T, G4double GlobalX, G4double GlobalY, G4double GlobalZ, G4double GlobalXPrime, G4double GlobalYPrime, G4double GlobalZPrime, G4double S, G4double Weight, G4int PDGtype, G4int EventNo, G4int ParentID, G4int TrackID, G4int TurnsTaken);
#endif
  
  void WriteAsciiHit(G4int PDGType, G4double Mom, G4double  X, G4double  Y, G4double  S, G4double  XPrime, G4double  YPrime, G4int EventNo, G4double  Weight, G4int  ParentID, G4int  TrackID, G4int TurnsTaken);


};

extern BDSOutput* bdsOutput;
#endif
