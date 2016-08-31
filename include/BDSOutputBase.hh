#ifndef BDSOutputBase_h
#define BDSOutputBase_h 

#include "globals.hh"

#include <ctime>
#include <string>
#include <vector>

// forward declarations
template <class T> class G4THitsCollection;
class BDSEnergyCounterHit;
typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
class BDSHistogram1D;
class BDSOutputROOTEventInfo;
class BDSSamplerHit;
typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
class BDSTrajectory;
class BDSTrajectoryPoint;
typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;
class BDSTunnelHit;
typedef G4THitsCollection<BDSTunnelHit> BDSTunnelHitsCollection;

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
  virtual void WritePrimaryLoss(BDSTrajectoryPoint* ploss) = 0;

  /// write where primaries impact
  virtual void WritePrimaryHit(BDSTrajectoryPoint* phits) = 0;

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
  virtual void WriteEventInfo(const time_t&  startTime,
			      const time_t&  stopTime,
			      const G4float& duration,
                              const std::string &seedStateAtStart) = 0;

  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* info) = 0;

  /// write a histgoram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn) = 0;
  
  /// write a complete event
  virtual void FillEvent() = 0;

  /// open new file
  virtual void Initialise()=0;
  
  /// write the data to file
  virtual void Write(const time_t&  startTime,
		     const time_t&  stopTime,
		     const G4float& duration,
                     const std::string& seedStateAtStart) = 0;

  /// close file
  virtual void Close() = 0;

  /// write, close and open new file
  void Commit(const time_t&  startTime,
	      const time_t&  stopTime,
	      const G4float& duration,
              const std::string& seedStateAtStart);
  
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
