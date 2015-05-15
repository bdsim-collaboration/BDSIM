#include "BDSBunchRing.hh"
#include "BDSDebug.hh"

BDSBunchRing::BDSBunchRing() : 
  BDSBunchInterface(), rMin(0), rMax(0) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
}

BDSBunchRing::BDSBunchRing(G4double rMinIn,   G4double rMaxIn,
			   G4double X0In,     G4double Y0In,       G4double Z0In,   G4double T0In, 
			   G4double Xp0In,    G4double Yp0In,      G4double Zp0In,			     
			   G4double sigmaTIn, G4double sigmaEIn) :
  BDSBunchInterface(X0In,Y0In,Z0In,T0In,Xp0In,Yp0In,Zp0In,sigmaTIn,sigmaEIn), rMin(rMinIn), rMax(rMaxIn) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
}

BDSBunchRing::~BDSBunchRing() {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  delete FlatGen;
}

void BDSBunchRing::SetOptions(struct Options& opt) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  SetRMin(opt.Rmin);  
  SetRMax(opt.Rmax);  
}

void BDSBunchRing::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				       G4double& xp, G4double& yp, G4double& zp,
				       G4double& t , G4double&  E, G4double& weight) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  double r = ( rMin + (rMax - rMin) *  rand() / RAND_MAX );
  double phi = 2 * CLHEP::pi * rand() / RAND_MAX;
     
  x0 = ( X0 + r * sin(phi) ) * CLHEP::m;
  y0 = ( Y0 + r * cos(phi) ) * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  zp = CalculateZp(xp,yp,Zp0);
  t  = T0 * CLHEP::s;
  E  = BDSGlobalConstants::Instance()->GetParticleKineticEnergy()* (1 + sigmaE/2. * (1. -2. * FlatGen->shoot()));  
  weight = 1.0;
}

