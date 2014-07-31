#include "BDSBunchEShell.hh"

BDSBunchEShell::BDSBunchEShell() : 
  BDSBunchInterface(), shellX(0.0), shellXp(0.0), shellY(0.0), shellYp(0.0) {
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine()); 
}

BDSBunchEShell::BDSBunchEShell(G4double shellXIn, G4double shellXpIn,
			       G4double shellYIn, G4double shellYpIn,			      
			       G4double X0In,     G4double Y0In,       G4double Z0In,   G4double T0In, 
			       G4double Xp0In,    G4double Yp0In,      G4double Zp0In,			     
			       G4double sigmaTIn, G4double sigmaEIn) :
  BDSBunchInterface(X0In,Y0In,Z0In,T0In,Xp0In,Yp0In,Zp0In,sigmaTIn,sigmaEIn), shellX(shellXIn), shellXp(shellXpIn), shellY(shellYIn), shellYp(shellYpIn)  
{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine()); 
}

BDSBunchEShell::~BDSBunchEShell() 
{
  delete FlatGen;
}

void BDSBunchEShell::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  SetShellX(opt.shellX);
  SetShellY(opt.shellY);
  SetShellXp(opt.shellXp);
  SetShellYp(opt.shellYp);
}

void BDSBunchEShell::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight) {
  G4double phi = 2 * CLHEP::pi * FlatGen->shoot();
  
  x0 = (X0 + sin(phi) * shellX) * CLHEP::m;
  xp = Xp0 + cos(phi) * shellXp;
  
  phi = 2 * CLHEP::pi * FlatGen->shoot();
  
  y0 = (Y0 + sin(phi) * shellY) * CLHEP::m;
  yp = Yp0 + cos(phi) * shellYp;
  
  z0 = Z0 * CLHEP::m;
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp = sqrt(1.-xp*xp -yp*yp);
  
  t = T0 * CLHEP::s;
  E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy()* (1 + sigmaE/2. * (1. -2. * FlatGen->shoot()));
  weight = 1.0;

  return;
}
		    
