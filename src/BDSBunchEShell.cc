#include "BDSBunchEShell.hh"
#include "BDSDebug.hh"

BDSBunchEShell::BDSBunchEShell(): 
  BDSBunchInterface(), shellX(0.0), shellXp(0.0), shellY(0.0), shellYp(0.0), 
  shellXWidth(0.0), shellXpWidth(0.0), shellYWidth(0.0), shellYpWidth(0.0) 
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine()); 
}

BDSBunchEShell::~BDSBunchEShell() 
{
  delete FlatGen;
}

void BDSBunchEShell::SetOptions(const GMAD::Options& opt)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBunchInterface::SetOptions(opt);
  SetShellX (opt.shellX);
  SetShellY (opt.shellY);
  SetShellXp(opt.shellXp);
  SetShellYp(opt.shellYp);
  SetShellXWidth (opt.shellXWidth );
  SetShellXpWidth(opt.shellXpWidth);
  SetShellYWidth (opt.shellYWidth );
  SetShellYpWidth(opt.shellYpWidth);
}

void BDSBunchEShell::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4double phi = 2 * CLHEP::pi * FlatGen->shoot();
  G4double xamp  = 0.5 - FlatGen->shoot();
  G4double yamp  = 0.5 - FlatGen->shoot();
  G4double xpamp = 0.5 - FlatGen->shoot();
  G4double ypamp = 0.5 - FlatGen->shoot();
  
  x0 = (X0 +  (sin(phi) * shellX)  + xamp * shellXWidth) * CLHEP::m;
  xp = (Xp0 + (cos(phi) * shellXp) + xpamp * shellXpWidth);
  
  phi = 2 * CLHEP::pi * FlatGen->shoot();
  
  y0 = (Y0 +  (sin(phi) * shellY)  + yamp * shellYWidth) * CLHEP::m;
  yp = (Yp0 + (cos(phi) * shellYp) + ypamp * shellYpWidth);
  
  z0 = Z0 * CLHEP::m;
  zp = CalculateZp(xp,yp,Zp0);
 
  t = T0 * CLHEP::s;
  E = BDSGlobalConstants::Instance()->GetParticleKineticEnergy()* (1 + sigmaE/2. * (1. -2. * FlatGen->shoot()));
  weight = 1.0;

  return;
}
		    
