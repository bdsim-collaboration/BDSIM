#ifndef BDSBUNCHSIXTRACK_H
#define BDSBUNCHSIXTRACK_H

#include <vector>

#include "BDSBunchInterface.hh"
#include "BDSDebug.hh"

/**
 * @brief A bunch distribution that reads a SixTrack hits file.
 * 
 * @author Regina Kwee-Hinzmann
 */

class BDSBunchSixTrack: public BDSBunchInterface
{ 
private: 
  G4int    nPart;
  G4String fileName;

  G4int    iPart; // current ray
  std::vector<double*> sixtrackData; 
  
public: 
  BDSBunchSixTrack();
  BDSBunchSixTrack(G4String fileNameIn);
  virtual ~BDSBunchSixTrack(); 
  void LoadSixTrackFile(); 
  virtual void SetOptions(const GMAD::Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
  void SetDistribFile(G4String distribFileNameIn) {fileName = distribFileNameIn;}
  G4String GetDistribFile() {return fileName;}
  G4int GetNParticles() {return nPart;}
};

#endif
