#ifndef BDSBunchPtc_h
#define BDSBunchPtc_h

#include <vector>

#include "BDSBunchInterface.hh"
#include "BDSDebug.hh"

class BDSBunchPtc : public BDSBunchInterface { 
private: 
  G4int    nRays;
  G4String fileName;

  G4int    iRay; ///< current ray
  std::vector<double*> ptcData; 

  void LoadPtcFile(); 
  void SetDistribFile(G4String distribFileNameIn);
  
public: 
  BDSBunchPtc();
  ~BDSBunchPtc(); 
  virtual void SetOptions(GMAD::Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
};

#endif
