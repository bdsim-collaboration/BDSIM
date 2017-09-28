#ifndef BDSBUNCHSIXTRACK_H
#define BDSBUNCHSIXTRACK_H

#include <vector>

#include "BDSBunch.hh"

/**
 * @brief A bunch distribution that reads a SixTrack hits file.
 * 
 * @author Regina Kwee-Hinzmann
 */

class BDSBunchSixTrack: public BDSBunch
{ 
private: 
  G4int    nPart;
  G4String fileName;

  G4int    iPart; // current ray
  std::vector<double*> sixtrackData; 
  
public: 
  BDSBunchSixTrack();
  explicit BDSBunchSixTrack(G4String fileNameIn);
  virtual ~BDSBunchSixTrack(); 
  void LoadSixTrackFile(); 
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
  inline void SetDistrFile(G4String distrFileNameIn) {fileName = distrFileNameIn;}
  G4String GetDistribFile() {return fileName;}
  G4int GetNParticles() {return nPart;}
};

#endif
