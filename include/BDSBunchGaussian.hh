#ifndef BDSBunchGaussian_h
#define BDSBunchGaussian_h 

#include "BDSBunchInterface.hh"
#include "Randomize.hh"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

namespace CLHEP {
  class RandGauss;
  class RandFlat;
  class RandMultiGauss;
}

class BDSBunchGaussian : public BDSBunchInterface {
private:
  /// Create multidimensional Gaussian random number generator
  void CreateMultiGauss();
 
protected : 
  G4double sigmaX;
  G4double sigmaY;
  G4double sigmaXp;
  G4double sigmaYp;

  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

  // Multidimensional Gaussian random number generator
  CLHEP::RandMultiGauss* GaussMultiGen;

public :
  BDSBunchGaussian();
  BDSBunchGaussian(G4double sigmaX, G4double sigmaY, G4double sigmaXp, G4double sigmaYp, 
		   G4double X0,     G4double Y0,     G4double Z0,  G4double T0,
		   G4double Xp0,    G4double Yp0,    G4double Zp0, 
		   G4double sigmaT, G4double sigmaE);
  BDSBunchGaussian(G4double *sigma, 
		   G4double X0,     G4double Y0,  G4double Z0,  G4double T0,
		   G4double Xp0,    G4double Yp0, G4double Zp0, 
		   G4double sigmaT, G4double sigmaE);
  ~BDSBunchGaussian();
  void SetOptions(struct Options& opt); 
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  

  G4double GetSigmaX()  {return sigmaX;}
  G4double GetSigmaY()  {return sigmaY;}
  G4double GetSigmaXp() {return sigmaXp;}
  G4double GetSigmaYp() {return sigmaYp;}
  G4double GetSigma(G4int i, G4int j) {return sigmaGM[i][j];}
  
  void SetSigmaX(G4double sigmaXIn) {sigmaX = sigmaXIn;}
  void SetSigmaY(G4double sigmaYIn) {sigmaY = sigmaYIn;}
  void SetSigmaXp(G4double sigmaXpIn) {sigmaXp = sigmaXpIn;}
  void SetSigmaYp(G4double sigmaYpIn) {sigmaYp = sigmaYpIn;}
  void SetSigma(G4int i, G4int j, G4double sigmaIn) {sigmaGM[i][j] = sigmaIn;}   
};

#endif
