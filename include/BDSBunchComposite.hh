#ifndef BDSBunchComposite_h
#define BDSBunchComposite_h 

#include "globals.hh"
#include "parser/options.h"
#include "BDSGlobalConstants.hh"

#include "BDSBunchInterface.hh"
#include "BDSBunchGaussian.hh"
#include "BDSBunchSquare.hh"
#include "BDSBunchCircle.hh"
#include "BDSBunchRing.hh"
#include "BDSBunchEShell.hh"
#include "BDSBunchTwiss.hh"
#include "BDSBunchOld.hh"
#include "BDSBunchUserFile.hh"

class BDSBunchComposite : public BDSBunchInterface {
 
protected : 
  BDSBunchInterface *xBunch; 
  BDSBunchInterface *yBunch;

public :
  BDSBunchComposite(); 
  ~BDSBunchComposite();
  void SetOptions(struct Options& opt); 
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  

  
};

#endif
