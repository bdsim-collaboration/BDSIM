#ifndef BDSOutputNone_h
#define BDSOutputNone_h 

#include "BDSOutput.hh"

class BDSOutputROOTEventInfo;

/**
 * @brief No output class
 * 
 * Write no output
 *
 * @author Jochem Snuverink
 */

class BDSOutputNone: public BDSOutput
{
public: 

  BDSOutputNone():BDSOutput(){} ///< default constructor
  virtual ~BDSOutputNone(){}

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*){}
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*){}
  /// write primary loss histo
  virtual void WritePrimaryLoss(BDSTrajectoryPoint*){}
  /// write primary hits histo
  virtual void WritePrimaryHit(BDSTrajectoryPoint*){}
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSEnergyCounterHitsCollection*){}
  /// write a trajectory
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &){}
  /// write primary hit
  virtual void WritePrimary(G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4int,G4int,G4int){}
  /// write event info
  virtual void WriteEventInfo(const time_t &, const time_t &, const G4float &, const std::string &)
  {}
  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* /*info*/){;}
  virtual void FillEvent(){}///< fill event structure
  virtual void Initialise(){}; ///< open the file
  virtual void Write(const time_t &, const time_t &, const G4float &, const std::string &)
  {};      ///< write to file
  virtual void Close(){};      ///< close the file
};

#endif
