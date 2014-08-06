#ifndef BDSBunchInterface_h
#define BDSBunchInterface_h 

#include "globals.hh"
#include "parser/options.h"
#include "BDSGlobalConstants.hh"

namespace CLHEP {
  class HepRandomEngine;
  class HepSymMatrix;
  class HepVector;
  class RandMultiGauss;
}

class BDSBunchInterface { 
protected : 
  // Centre of distributions
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double T0; 
  G4double Xp0; 
  G4double Yp0;
  G4double Zp0;
  G4double sigmaT; 
  G4double sigmaE;

  /// Create multidimensional Gaussian random number generator
  // for Twiss and Gauss, could be moved elsewhere
  // can change sigma matrix to make non-definite
  CLHEP::RandMultiGauss* CreateMultiGauss(CLHEP::HepRandomEngine & anEngine, const CLHEP::HepVector & mu, CLHEP::HepSymMatrix & sigma);

public : 
  BDSBunchInterface();
  BDSBunchInterface(G4double sigmaT, G4double sigmaE); 
  BDSBunchInterface(G4double X0,     G4double Y0,     G4double Z0, G4double T0,
		    G4double Xp0,    G4double Yp0,    G4double Zp0, 
		    G4double sigmaT, G4double sigmaE);
  virtual ~BDSBunchInterface();
  virtual void SetOptions(struct Options& opt); 
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

  G4double GetX0()     {return X0;}
  G4double GetY0()     {return Y0;}
  G4double GetZ0()     {return Z0;}
  G4double GetT0()     {return T0;}
  G4double GetXp0()    {return Xp0;}
  G4double GetYp0()    {return Yp0;}
  G4double GetZp0()    {return Zp0;}
  G4double GetSigmaE() {return sigmaE;}
  G4double GetSigmaT() {return sigmaT;}
};

#endif
