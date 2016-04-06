#include "BDSBunchSquare.hh"
#include "BDSDebug.hh"

BDSBunchSquare::BDSBunchSquare() :
  BDSBunchInterface(), envelopeX(0.0), envelopeY(0.0), 
  envelopeXp(0.0), envelopeYp(0.0), envelopeT(0.0), envelopeE(0.0)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchSquare::~BDSBunchSquare()
{
  delete FlatGen;
}

void BDSBunchSquare::SetOptions(const GMAD::Options& opt)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  SetEnvelopeX(opt.envelopeX); 
  SetEnvelopeY(opt.envelopeY);
  SetEnvelopeXp(opt.envelopeXp);
  SetEnvelopeYp(opt.envelopeYp);
  SetEnvelopeT(opt.envelopeT);
  SetEnvelopeE(opt.envelopeE);
  return; 
}

void BDSBunchSquare::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  x0 = X0  * CLHEP::m;
  y0 = Y0  * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  z0 = Z0  * CLHEP::m + (T0 - envelopeT * (1.-2.*FlatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  
  if(envelopeX !=0) x0  += envelopeX  * (1-2*FlatGen->shoot()) * CLHEP::m;
  if(envelopeY !=0) y0  += envelopeY  * (1-2*FlatGen->shoot()) * CLHEP::m;
  if(envelopeXp !=0) xp += envelopeXp * (1-2*FlatGen->shoot()) * CLHEP::rad;
  if(envelopeYp !=0) yp += envelopeYp * (1-2*FlatGen->shoot()) * CLHEP::rad;
  
  zp = CalculateZp(xp,yp,Zp0);
  t = 0 * CLHEP::s;
  E = BDSGlobalConstants::Instance()->GetParticleTotalEnergy() * (1 + envelopeE * (1-2*FlatGen->shoot()));

  weight = 1.0;
  return; 
}
