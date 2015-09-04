#ifndef BDSBunchComposite_h
#define BDSBunchComposite_h 

#include "globals.hh"
#include "parser/options.h"

#include "BDSBunchInterface.hh"

class BDSBunchComposite : public BDSBunchInterface {
 
protected : 
  BDSBunchInterface *xBunch; 
  BDSBunchInterface *yBunch;
  BDSBunchInterface *zBunch;

public :
  BDSBunchComposite(); 
  ~BDSBunchComposite();
  void SetOptions(GMAD::Options& opt); 
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  

  
};

#endif
