#ifndef BDSBUNCHSQUARE_H
#define BDSBUNCHSQUARE_H 

#include "BDSBunch.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief A bunch distribution that produces an uncorrelated uniform
 * random distribution within a square in phase space.
 * 
 * @author Stewart Boogert
 */

class BDSBunchSquare: public BDSBunch
{ 
protected : 
  G4double envelopeX; 
  G4double envelopeY;
  G4double envelopeXp; 
  G4double envelopeYp; 
  G4double envelopeT;
  G4double envelopeE; 
  
  CLHEP::RandFlat* FlatGen;    

public: 
  BDSBunchSquare(); 
  virtual ~BDSBunchSquare(); 
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  
protected:
  void SetEnvelopeX(G4double envelopeXIn)  {envelopeX = envelopeXIn;}
  void SetEnvelopeY(G4double envelopeYIn)  {envelopeY = envelopeYIn;}
  void SetEnvelopeXp(G4double envelopeXpIn){envelopeXp= envelopeXpIn;}
  void SetEnvelopeYp(G4double envelopeYpIn){envelopeYp= envelopeYpIn;}
  void SetEnvelopeT(G4double envelopeTIn)  {envelopeT = envelopeTIn;}
  void SetEnvelopeE(G4double envelopeEIn)  {envelopeE = envelopeEIn;}



};

#endif
