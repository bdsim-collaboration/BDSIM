#include "BDSBunchEShell.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchEShell::BDSBunchEShell(): 
  BDSBunch(), shellX(0.0), shellXp(0.0), shellY(0.0), shellYp(0.0),
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

void BDSBunchEShell::SetOptions(const GMAD::Beam& beam,
				G4Transform3D beamlineTransformIn)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBunch::SetOptions(beam, beamlineTransformIn);
  SetShellX (beam.shellX);
  SetShellY (beam.shellY);
  SetShellXp(beam.shellXp);
  SetShellYp(beam.shellYp);
  SetShellXWidth (beam.shellXWidth );
  SetShellXpWidth(beam.shellXpWidth);
  SetShellYWidth (beam.shellYWidth );
  SetShellYpWidth(beam.shellYpWidth);
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

  ApplyTransform(x0,y0,z0,xp,yp,zp);
 
  t = T0 * CLHEP::s;
  E = E0 * CLHEP::GeV * (1 + sigmaE/2. * (1. -2. * FlatGen->shoot()));
  weight = 1.0;

  return;
}
		    
