#ifndef BDSBUNCHRING_H
#define BDSBUNCHRING_H 

#include "BDSBunch.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief A bunch distribution that produces an uncorrelated random
 * uniform distribution along a circle in phase space.
 * 
 * @author Stewart Boogert
 */

class BDSBunchRing: public BDSBunch
{ 
protected : 
  G4double rMin;
  G4double rMax;
  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchRing(); 
  ~BDSBunchRing(); 
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

protected:
  void SetRMin(G4double rMinIn) { rMin = rMinIn;}
  void SetRMax(G4double rMaxIn) { rMax = rMaxIn;}
  
};

#endif
