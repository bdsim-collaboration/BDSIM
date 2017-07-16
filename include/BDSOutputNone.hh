#ifndef BDSOutputNone_h
#define BDSOutputNone_h 

#include "BDSOutput.hh"

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
  BDSOutputNone(): BDSOutput("", -1){;} ///< Default constructor.
  virtual ~BDSOutputNone(){;}

  /// @{ No action.
  virtual void NewFile(){;}
  virtual void InitialiseGeometryDependent(){;}
  virtual void CloseFile(){;}
  virtual G4bool FileIsOpen() {return false;}
  virtual void FillEventInfo(const BDSEventInfo *){;}
  virtual void FillSamplerHits(const BDSSamplerHitsCollection *, const HitsType){;}
  virtual void FillEnergyLoss(const BDSEnergyCounterHitsCollection *, const LossType){;}
  virtual void FillPrimaryHit(const BDSTrajectoryPoint *){;}
  virtual void FillPrimaryLoss(const BDSTrajectoryPoint *){;}
  virtual void FillTrajectories(const std::vector<BDSTrajectory *> &){;}
  virtual void FillRunInfo(const BDSEventInfo *){;}
  virtual void WriteFileEventLevel(){}
  virtual void WriteFileRunLevel(){}
  virtual void FillPrimary(const G4double, const G4double, const G4double,
			   const G4double, const G4double, const G4double,
			   const G4double, const G4double, const G4double,
			   const G4int,    const G4int,    const G4int){;}
  /// @}
};

#endif
