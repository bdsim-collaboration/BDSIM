#ifndef BDSBUNCHCIRCLE_H
#define BDSBUNCHCIRCLE_H 

#include "BDSBunchInterface.hh"
#include "Randomize.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief An uncorrelated uniform random distribution within a circle in each dimension.
 * 
 * @author Stewart Boogert <Stewart.Boogert@rhul.ac.uk>
 */

class BDSBunchCircle: public BDSBunchInterface
{
protected : 
  G4double envelopeR; 
  G4double envelopeRp; 
  G4double envelopeT;
  G4double envelopeE; 
  
  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchCircle(); 
  ~BDSBunchCircle(); 
  void SetOptions(const GMAD::Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  
protected:
  void SetEnvelopeR(G4double envelopeRIn)  {envelopeR = envelopeRIn;}
  void SetEnvelopeRp(G4double envelopeRpIn) {envelopeRp= envelopeRpIn;}
  void SetEnvelopeT(G4double envelopeTIn)  {envelopeT = envelopeTIn;}
  void SetEnvelopeE(G4double envelopeEIn)  {envelopeE = envelopeEIn;}



};

#endif
