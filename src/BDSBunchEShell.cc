#include "BDSBunchERing.hh"

BDSBunchERing::BDSBunchERing() : 
  BDSBunchInterface() {}

BDSBunchERing::BDSBunchERing() :
  BDSBunchInterface() {}

BDSBunchERing::~BDSBunchERing() : 
{
}

void BDSBunchRing::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  SetRMin(opt.Rmin);  
  SetRMax(opt.Rmax);  
}

void BDSBunchRing::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				       G4double& xp, G4double& yp, G4double& zp,
				       G4double& t , G4double&  E, G4double& weight) {
  double r = ( rMin + (rMax - rMin) *  rand() / RAND_MAX );
  double phi = 2 * CLHEP::pi * rand() / RAND_MAX;
     
  x0 = ( X0 + r * sin(phi) ) * CLHEP::m;
  y0 = ( Y0 + r * cos(phi) ) * CLHEP::m;
  z0 = Z0 * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp = sqrt(1.-xp*xp -yp*yp);
  t = T0 * CLHEP::s;
  E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy()* (1 + sigmaE/2. * (1. -2. * FlatGen->shoot()));  
  weight = 1.0;
}
