#ifndef BDSOutputBase_h
#define BDSOutputBase_h 

#include "BDSSamplerHit.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSTrajectory.hh"

#include "G4Trajectory.hh"
#include <vector>

#include "tracker/TRKBunch.hh"

// virtual base class

class BDSOutputBase {

public: 

  BDSOutputBase(); // default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputBase(){};

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*) = 0;
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*) = 0;
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec) = 0;
  /// write primary hit
  virtual void WritePrimary(G4String samplerName, 
			    G4double E,
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

  /// write tracker bunch
  virtual void WriteTrackerBunch(G4String samplerName, TRKBunch* bunch, bool primary=false) = 0;
  /// write and close and open new file
  virtual void Commit()=0;
  /// write and close the file
  virtual void Write()=0;

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
