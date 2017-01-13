#ifndef BDSBUNCHPTC_H
#define BDSBUNCHPTC_H

#include <vector>

#include "BDSBunchInterface.hh"
#include "BDSDebug.hh"

/**
 * @brief A bunch distribution that reads a PTC inrays file.
 * 
 * @author Stewart Boogert
 */

class BDSBunchPtc: public BDSBunchInterface
{ 
private: 
  G4int    nRays;
  G4String fileName;

  G4int    iRay; ///< current ray
  std::vector<double*> ptcData; 

  void LoadPtcFile(); 
  void SetDistribFile(G4String distribFileNameIn);
  
public: 
  BDSBunchPtc();
  virtual ~BDSBunchPtc(); 
  virtual void SetOptions(const GMAD::Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
};

#endif
