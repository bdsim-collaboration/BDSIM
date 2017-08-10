#ifndef BDSBUNCHHALO_H
#define BDSBUNCHHALO_H

#include "BDSBunch.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief A halo distribution based on both twiss parameters and sigmas.
 *
 * @author Stewart Boogert
 */

class BDSBunchHalo: public BDSBunch
{
private: 
  /* Twiss parameters */
  G4double betaX;
  G4double betaY;
  G4double alphaX;
  G4double alphaY;
  G4double emitX;
  G4double emitY;
  G4double gammaX;
  G4double gammaY;

  G4double envelopeX; 
  G4double envelopeY;
  G4double envelopeXp; 
  G4double envelopeYp; 

  G4double envelopeEmitX;
  G4double envelopeEmitY;

  G4double envelopeCollMinX;
  G4double envelopeCollMaxX;
  G4double envelopeCollMinXp;
  G4double envelopeCollMaxXp;

  G4double envelopeCollMinY;
  G4double envelopeCollMaxY;
  G4double envelopeCollMinYp;
  G4double envelopeCollMaxYp;

  CLHEP::RandFlat  *FlatGen;

  G4double weightParameter;
  std::string weightFunction;

  /// @{ Whether to generate the initial area of randomly uniform coordinates
  /// in two halves for efficiency or not.
  G4bool twoLobeX;
  G4bool twoLobeY;
  /// @}

  G4double xMinDist; ///< Distance from -ve x collimator lower limit to outer envelope.
  G4double xMaxDist; ///< Distance from +ve x collimator upper limit to outer envelope.
  G4double xMinMaxRatio; ///< Normalised ratio of x lobe areas for proportioning.

  G4double yMinDist; ///< Distance from -ve y collimator lower limit to outer envelope.
  G4double yMaxDist; ///< Distance from +ve y collimator upper limit to outer envelope.
  G4double yMinMaxRatio; ///< Normalised ratio of y lobe areas for proportioning.
  
public: 
  BDSBunchHalo();
  virtual ~BDSBunchHalo();
  virtual void SetOptions(const GMAD::Options& opt,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  

protected:
  void     SetBetaX(double newBetaX)                          {betaX  = newBetaX;}
  void     SetBetaY(double newBetaY)                          {betaY  = newBetaY;}
  void     SetAlphaX(double newAlphaX)                        {alphaX = newAlphaX;}
  void     SetAlphaY(double newAlphaY)                        {alphaY = newAlphaY;}
  void     SetEmitX(double newEmitX)                          {emitX  = newEmitX;}
  void     SetEmitY(double newEmitY)                          {emitY  = newEmitY;}
  void     SetEnvelopeX(G4double envelopeXIn)                 {envelopeX  = envelopeXIn;}
  void     SetEnvelopeY(G4double envelopeYIn)                 {envelopeY  = envelopeYIn;}
  void     SetEnvelopeXp(G4double envelopeXpIn)               {envelopeXp = envelopeXpIn;}
  void     SetEnvelopeYp(G4double envelopeYpIn)               {envelopeYp = envelopeYpIn;}
  void     SetEnvelopeEmitX(G4double envelopeEmitXIn)         {envelopeEmitX = envelopeEmitXIn;}
  void     SetEnvelopeEmitY(G4double envelopeEmitYIn)         {envelopeEmitY = envelopeEmitYIn;}
  void     SetEnvelopeCollMinX(G4double envelopeCollMinXIn)   {envelopeCollMinX  = envelopeCollMinXIn;}
  void     SetEnvelopeCollMaxX(G4double envelopeCollMaxXIn)   {envelopeCollMaxX  = envelopeCollMaxXIn;}
  void     SetEnvelopeCollMinXp(G4double envelopeCollMinXpIn) {envelopeCollMinXp = envelopeCollMinXpIn;}
  void     SetEnvelopeCollMaxXp(G4double envelopeCollMaxXpIn) {envelopeCollMaxXp = envelopeCollMaxXpIn;}
  void     SetEnvelopeCollMinY(G4double envelopeCollMinYIn)   {envelopeCollMinY  = envelopeCollMinYIn;}
  void     SetEnvelopeCollMaxY(G4double envelopeCollMaxYIn)   {envelopeCollMaxY  = envelopeCollMaxYIn;}
  void     SetEnvelopeCollMinYp(G4double envelopeCollMinYpIn) {envelopeCollMinYp = envelopeCollMinYpIn;}
  void     SetEnvelopeCollMaxYp(G4double envelopeCollMaxYpIn) {envelopeCollMaxYp = envelopeCollMaxYpIn;}
  void     SetWeightParameter(G4double haloPSWeightParameter) {weightParameter  = haloPSWeightParameter;}
  void     SetWeightFunction(std::string haloPSWeightFunction) {
    if(haloPSWeightFunction == "flat" || haloPSWeightFunction == "oneoverr" || haloPSWeightFunction == "exp") {
      weightFunction = haloPSWeightFunction;
    }
    else {
      weightFunction = ""; 
    }
  }
};

#endif
