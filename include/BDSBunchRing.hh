#ifndef BDSBunchRing_h
#define BDSBunchRing_h 

#include "BDSBunchInterface.hh"
#include "Randomize.hh"

namespace CLHEP {
  class RandFlat;
}

class BDSBunchRing : public BDSBunchInterface { 
protected : 
  G4double rMin;
  G4double rMax;
  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchRing(); 
  BDSBunchRing(G4double rMin, G4double rMax,
	       G4double X0,     G4double Y0,    G4double Z0,   G4double T0, 
	       G4double Xp0,    G4double Yp0,   G4double Zp0,
	       G4double sigmaT, G4double sigmaE);
  ~BDSBunchRing(); 
  void SetOptions(struct Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

  G4double GetRMin() {return rMin;}
  G4double GetRMax() {return rMax;}
protected:
  void SetRMin(G4double rMinIn) { rMin = rMinIn;}
  void SetRMax(G4double rMaxIn) { rMax = rMaxIn;}
  
};

#endif
