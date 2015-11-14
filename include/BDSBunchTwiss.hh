#ifndef BDSBunchTwiss_h
#define BDSBunchTwiss_h 

#include "BDSBunchInterface.hh"
#include "Randomize.hh"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

/**
 * @brief A bunch distribution according to the twiss parameterisation.
 * 
 * @author Stewart Boogert <Stewart.Boogert@rhul.ac.uk>
 */

class BDSBunchTwiss: public BDSBunchInterface
{
private : 
  /* Twiss parameters */
  G4double betaX;
  G4double betaY;
  G4double alphaX;
  G4double alphaY;
  G4double emitX;
  G4double emitY; 
  G4double gammaX;
  G4double gammaY;
  
  /* Random number generators */
  //  CLHEP::RandGauss *GaussGen;
  //  CLHEP::RandFlat  *FlatGen;  
  CLHEP::RandMultiGauss* GaussMultiGen;  

  /* Gaussian generator */
  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

public : 
  BDSBunchTwiss();
  ~BDSBunchTwiss();
  void SetOptions(GMAD::Options &opt); 
  void CommonConstruction();
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  G4double GetBetaX()  {return betaX;}
  G4double GetBetaY()  {return betaY;}
  G4double GetAlphaX() {return alphaX;}
  G4double GetAlphaY() {return alphaY;} 
  G4double GetEmitX()  {return emitX;}
  G4double GetEmitY()  {return emitY;}

private:
  void     SetBetaX(double newBetaX) {betaX    = newBetaX;}
  void     SetBetaY(double newBetaY) {betaY    = newBetaY;}
  void     SetAlphaX(double newAlphaX) {alphaX = newAlphaX;}
  void     SetAlphaY(double newAlphaY) {alphaY = newAlphaY;}
  void     SetEmitX(double newEmitX)   {emitX  = newEmitX;}
  void     SetEmitY(double newEmitY)   {emitY  = newEmitY;}   
};

#endif
