#include "BDSBunchCircle.hh"

BDSBunchCircle::BDSBunchCircle() :
  BDSBunchInterface(), envelopeR(0.0), envelopeRp(0.0), envelopeT(0.0), envelopeE(0.0)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchCircle::BDSBunchCircle(G4double envelopeRIn, G4double envelopeRpIn,
			       G4double envelopeTIn, G4double envelopeEIn,
			       G4double X0In,        G4double Y0In,         G4double Z0In,   G4double T0In, 
			       G4double Xp0In,       G4double Yp0In,        G4double Zp0In) : 
  BDSBunchInterface(X0In,Y0In,Z0In,T0In,Xp0In,Yp0In,Zp0In,0.0,0.0),
  envelopeR(envelopeRIn), envelopeRp(envelopeRpIn), envelopeT(envelopeTIn), envelopeE(envelopeEIn)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchCircle::~BDSBunchCircle() 
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  delete FlatGen;
}

void BDSBunchCircle::SetOptions(struct Options &opt) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  SetEnvelopeR(opt.envelopeX); 
  SetEnvelopeRp(opt.envelopeXp);
  SetEnvelopeT(opt.envelopeT);
  SetEnvelopeE(opt.envelopeE);
  return; 
}

void BDSBunchCircle::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  x0 = X0 * CLHEP::m;
  y0 = Y0 * CLHEP::m;
  z0 = Z0 * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  z0 = Z0 * CLHEP::m + (T0 - envelopeT * (1.-2.*FlatGen->shoot())) * CLHEP::c_light * CLHEP::s;

  G4double phiR  = FlatGen->shoot()*2*CLHEP::pi;
  G4double phiRp = FlatGen->shoot()*2*CLHEP::pi;
  G4double r     = FlatGen->shoot()*envelopeR;
  G4double rp    = FlatGen->shoot()*envelopeRp; 

  x0 += cos(phiR) * r * CLHEP::m;
  y0 += sin(phiR) * r * CLHEP::m;
  xp += cos(phiRp) * rp * CLHEP::rad;
  yp += sin(phiRp) * rp * CLHEP::rad;
  
  zp = CalculateZp(xp,yp,Zp0);
  t = 0.0;
  E = BDSGlobalConstants::Instance()->GetParticleKineticEnergy() * (1 + envelopeE * (1-2*FlatGen->shoot()));

  weight = 1.0;
  return; 
}
