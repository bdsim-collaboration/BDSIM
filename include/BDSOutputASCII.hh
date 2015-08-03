#ifndef BDSOutputASCII_h
#define BDSOutputASCII_h 

#include "BDSOutputBase.hh"

#include <fstream>

#include "tracker/TRKBunch.hh"

/**
 * @brief ASCII output class
 * 
 * Write BDSIM output to multiple ascii text files. Originally
 * part of BDSIM code base and recently developed and maintained
 * by Jochem Snuverink & Laurie Nevay
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSOutputASCII: public BDSOutputBase
{
public: 

  BDSOutputASCII(); ///> default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputASCII();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*);
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  /// write primary loss histo
  virtual void WritePrimaryLoss(BDSEnergyCounterHit*);
  /// write primary hits histo
  virtual void WritePrimaryHit(BDSEnergyCounterHit*);
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSTunnelHitsCollection*);
  /// write a trajectory
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec);
  /// write primary hit
  virtual void WritePrimary(G4String samplerName, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double zp,G4double t,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken);
  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn);
  virtual void Commit();  ///> close the file
  virtual void Write();   ///> close and open new file

  /// write tracker bunch
  virtual void WriteTrackerBunch(G4String samplerName, TRKBunch* bunch, bool primary=false);
  virtual void WriteTrackerPrimaryLoss(TRKBunch* lostBunch, int turnsTaken);
  
private:
  G4String basefilename;
  G4String timestring;
  
  /// main output file
  std::ofstream ofMain;
  /// primaries output file
  std::ofstream ofPrimaries;
  /// energy loss hits output file
  std::ofstream ofELoss;
  /// primary loss hits output file
  std::ofstream ofPLoss;

  void WriteAsciiHit(std::ofstream* outfile, 
		     G4int    PDGType, 
		     G4double Mom, 
		     G4double X, 
		     G4double Y,
		     G4double Z,
		     G4double S, 
		     G4double XPrime, 
		     G4double YPrime, 
		     G4int    EventNo, 
		     G4double Weight, 
		     G4int    ParentID, 
		     G4int    TrackID, 
		     G4int    TurnsTaken);
};

extern BDSOutputBase* bdsOutput;
#endif
