#ifndef BDSOutputASCII_h
#define BDSOutputASCII_h 

#include "BDSOutputBase.hh"

#include <fstream>

// ASCII output class

class BDSOutputASCII : public BDSOutputBase {

public: 

  BDSOutputASCII(); // default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputASCII();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*);
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec);
  /// write primary hit
  virtual void WritePrimary(G4String samplerName, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double zp,G4double t,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken);

  virtual void Commit();  /// close the file
  virtual void Write();   /// close and open new file

private:

  std::ofstream of;
  std::ofstream ofEloss;

  void WriteAsciiHit(G4int PDGType, G4double Mom, G4double X, G4double Y, G4double S, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken);

};

extern BDSOutputBase* bdsOutput;
#endif
