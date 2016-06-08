#ifndef BDSOutputBase_h
#define BDSOutputBase_h 

#include "BDSSamplerHit.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSTrajectory.hh"
#include "BDSTunnelHit.hh"
#include "BDSHistogram.hh"
#include "BDSOutputROOTEventHistograms.hh"

#include <vector>

/**
 * @brief Output base class that defines interface for all output types.
 */

class BDSOutputBase
{
public: 
  BDSOutputBase(); // default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputBase() {};

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*) = 0;
  
  /// write energy deposition hits
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*) = 0;
  
  /// write where primaries stop being primaries
  virtual void WritePrimaryLoss(BDSEnergyCounterHit* ploss) = 0;

  /// write where primaries impact
  virtual void WritePrimaryHit(BDSEnergyCounterHit* phits) = 0;

  /// write tunnel hits
  virtual void WriteTunnelHits(BDSTunnelHitsCollection* tunnelHits) = 0;
  
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec) = 0;
  
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
			    G4int    TurnsTaken) = 0;

  /// Write additional information about event such as timing.
  virtual void WriteEventInfo(time_t startTime, time_t stopTime, G4float duration) = 0;

  /// write a histgoram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn) = 0;
  
  /// write a complete event
  virtual void FillEvent() = 0;

  /// open new file
  virtual void Initialise()=0;
  
  /// write the data to file
  virtual void Write(const time_t& startTime, const time_t& stopTime, const G4float& duration) = 0;

  /// close file
  virtual void Close() = 0;

  /// write, close and open new file
  void Commit(const time_t& startTime, const time_t& stopTime, const G4float& duration);
  
protected:
  /// current event number
  int eventNumber;
  /// number of events stored per file
  int numberEventPerFile;
  ///number of output file
  int outputFileNumber;
  /// file name
  G4String filename;
};

extern BDSOutputBase* bdsOutput;
#endif
