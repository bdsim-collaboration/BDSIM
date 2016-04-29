#ifndef BDSOUTPUTVECTOR_H
#define BDSOUTPUTVECTOR_H 

#include "BDSOutputBase.hh"

#include <vector>

// class that holds multiple outputs

class BDSOutputVector: public BDSOutputBase {

public: 

  BDSOutputVector();
  ~BDSOutputVector();

  /// add output type
  void Add(BDSOutputBase*);

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*);
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  /// make primary loss histo
  virtual void WritePrimaryLoss(BDSEnergyCounterHit*);
  /// make primary hits histo - where primaries impact
  virtual void WritePrimaryHit(BDSEnergyCounterHit*);
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSTunnelHitsCollection*);
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

  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn);
  /// write event info
  virtual void WriteEventInfo(time_t startTime, time_t stopTime);
  /// Fill event
  virtual void FillEvent() {};
  virtual void Initialise(); ///< open the file
  virtual void Write();      ///< write to file
  virtual void Close();      ///< close the file

private:
  /// 
  std::vector<BDSOutputBase*> output;
};

#endif
