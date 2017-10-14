#ifndef BDSBUNCHCOMPOSITE_H
#define BDSBUNCHCOMPOSITE_H 

#include "globals.hh"

#include "BDSBunch.hh"

/**
 * @brief A distribution that allows mixing of three different 
 * distributions in each primary dimension.
 * 
 * @author Stewart Boogert
 */

class BDSBunchComposite: public BDSBunch
{ 
protected:
  BDSBunch* xBunch;
  BDSBunch* yBunch;
  BDSBunch* zBunch;

public:
  BDSBunchComposite(); 
  virtual ~BDSBunchComposite();
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity); 
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);
  
};

#endif
