#ifndef BDSBUNCHPTC_H
#define BDSBUNCHPTC_H

#include "BDSBunchInterface.hh"

#include "globals.hh"

#include <vector>

/**
 * @brief A bunch distribution that reads a PTC inrays file.
 * 
 * @author Stewart Boogert
 */

class BDSBunchPtc: public BDSBunchInterface
{  
public: 
  BDSBunchPtc();
  virtual ~BDSBunchPtc(); 
  virtual void SetOptions(const GMAD::Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
  
private:
  /// Load the PTC file into memory
  void LoadPtcFile();

  /// Assign the distribution file by finding the full path of it.
  void SetDistribFile(G4String distribFileNameIn);
  
  G4int    nRays;     ///< Number of rays in file (1 counting).
  G4String fileName;  ///< File name.
  G4int    iRay;      ///< Iterator counter for current ray.
  std::vector<double*> ptcData; ///< Data.
};

#endif
