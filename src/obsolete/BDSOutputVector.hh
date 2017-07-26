#ifndef BDSOUTPUTVECTOR_H
#define BDSOUTPUTVECTOR_H 

#include "BDSOutputBase.hh"

#include "globals.hh" // geant4 types / globals

#include <ctime>
#include <string>
#include <vector>

class BDSHistogram1D;
class BDSOutputROOTEventInfo;
class BDSTrajectory;
class BDSTrajectoryPoint;

/**
 * @brief Class that holds multiple outputs.
 * 
 * @author Jochem Snuverink
 */

class BDSOutputVector: public BDSOutputBase
{
public: 
  BDSOutputVector();
  virtual ~BDSOutputVector();

  /// add output type
  void Add(BDSOutputBase*);

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*);
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  /// make primary loss histo
  virtual void WritePrimaryLoss(BDSTrajectoryPoint* ploss);
  /// make primary hits histo - where primaries impact
  virtual void WritePrimaryHit(BDSTrajectoryPoint* phits);
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSEnergyCounterHitsCollection*);
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
  /// write tracker bunch
  virtual void WriteTrackerBunch(G4String samplerName,
				 TRKBunch* bunch,
				 bool primary=false);

  /// write tracker primary particles
  virtual void WriteTrackerPrimaryLoss(TRKBunch* lostBunch, int turnsTaken);

  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn);
  /// write event info
  virtual void WriteEventInfo(const time_t&  startTime,
			      const time_t&  stopTime,
			      const G4float& duration,
                              const std::string& seedStateAtStart);
  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* info);
  /// Fill event
  virtual void FillEvent() {};
  virtual void Initialise(); ///< open the file
  /// Write to file
  virtual void Write(const time_t&  startTime,
		     const time_t&  stopTime,
		     const G4float& duration,
		     const std::string& seedStateAtStart);
  virtual void Close();      ///< close the file

private:
  /// Vector of output instances.
  std::vector<BDSOutputBase*> output;
};

#endif
