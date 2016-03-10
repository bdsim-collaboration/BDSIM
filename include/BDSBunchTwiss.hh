#ifndef BDSBUNCHTWISS_H
#define BDSBUNCHTWISS_H 

#include "BDSBunchInterface.hh"
#include "Randomize.hh"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

/**
 * @brief A bunch distribution according to the twiss parameterisation.
 * 
 * @author Stewart Boogert
 */

class BDSBunchTwiss: public BDSBunchInterface
{
private: 
  /// @{ Twiss parameters
  G4double betaX;
  G4double betaY;
  G4double alphaX;
  G4double alphaY;
  G4double emitX;
  G4double emitY; 
  G4double gammaX;
  G4double gammaY;
  G4double dispX;
  G4double dispY;
  G4double dispXP;
  G4double dispYP;
  /// @}
  
  /// Random number generators 
  CLHEP::RandMultiGauss* GaussMultiGen;  

  /// Gaussian generator
  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

public: 
  BDSBunchTwiss();
  virtual ~BDSBunchTwiss();
  
  void SetOptions(const GMAD::Options& opt); 
  void CommonConstruction();
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

  /// @{ Accessor
  G4double GetBetaX()  const {return betaX;}
  G4double GetBetaY()  const {return betaY;}
  G4double GetAlphaX() const {return alphaX;}
  G4double GetAlphaY() const {return alphaY;} 
  G4double GetEmitX()  const {return emitX;}
  G4double GetEmitY()  const {return emitY;}
  G4double GetDispX()  const {return dispX;}
  G4double GetDispY()  const {return dispY;} 
  G4double GetDispXP() const {return dispXP;}
  G4double GetDispYP() const {return dispYP;}
  /// @}

private:
  /// @{ Setter
  void SetBetaX(double newBetaX)   {betaX  = newBetaX;}
  void SetBetaY(double newBetaY)   {betaY  = newBetaY;}
  void SetAlphaX(double newAlphaX) {alphaX = newAlphaX;}
  void SetAlphaY(double newAlphaY) {alphaY = newAlphaY;}
  void SetEmitX(double newEmitX)   {emitX  = newEmitX;}
  void SetEmitY(double newEmitY)   {emitY  = newEmitY;}
  void SetDispX(double newDispX)   {dispX  = newDispX;}
  void SetDispY(double newDispY)   {dispY  = newDispY;}
  void SetDispXP(double newDispXP) {dispXP = newDispXP;}
  void SetDispYP(double newDispYP) {dispXP = newDispYP;}
  /// @}
};

#endif
