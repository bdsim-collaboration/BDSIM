#ifndef BDSBUNCHTWISS_H
#define BDSBUNCHTWISS_H 

#include "BDSBunch.hh"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"

namespace CLHEP {
  class RandMultiGauss;
}

/**
 * @brief A bunch distribution according to the twiss parameterisation.
 * 
 * @author Stewart Boogert
 */

class BDSBunchTwiss: public BDSBunch
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
  
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity); 
  void CommonConstruction();
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

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
