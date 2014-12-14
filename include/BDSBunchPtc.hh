#ifndef BDSBunchPtc_h
#define BDSBunchPtc_h

#include <vector>

#include "BDSBunchInterface.hh"

class BDSBunchPtc : public BDSBunchInterface { 
private: 
  int nRays;
  G4String fileName;

  std::vector<double*> ptcData; 
  
public: 
  BDSBunchPtc();
  BDSBunchPtc(G4String fileNameIn);
  ~BDSBunchPtc(); 
  void LoadPtcFile(); 
  virtual void SetOptions(struct Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
  void SetDistribFile(G4String distribFileNameIn) {fileName = distribFileNameIn;}
  G4String GetDistribFile() {return fileName;}
  G4int GetNParticles() {return nRays;}
};

#endif
