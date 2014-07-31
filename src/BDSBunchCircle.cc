#include "BDSBunchCircle.hh"

BDSBunchCircle::BDSBunchCircle() :
  BDSBunchInterface(), envelopeX(0.0), envelopeY(0.0), 
  envelopeXp(0.0), envelopeYp(0.0), envelopeT(0.0), envelopeE(0.0)
{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchCircle::BDSBunchCircle(G4double envelopeXIn, G4double envelopeYIn,
			       G4double envelopeXpIn, G4double envelopeYpIn,
			       G4double envelopeTIn, G4double envelopeEIn) : 
  BDSBunchInterface(), envelopeX(envelopeXIn), envelopeY(envelopeYIn), 
  envelopeXp(envelopeXpIn), envelopeYp(envelopeYpIn), envelopeT(envelopeTIn), envelopeE(envelopeEIn)
{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchCircle::~BDSBunchCircle() 
{
  delete FlatGen;
}

void BDSBunchCircle::SetOptions(struct Options &opt) {
  SetEnvelopeX(opt.envelopeX); 
  SetEnvelopeY(opt.envelopeY);
  SetEnvelopeXp(opt.envelopeXp);
  SetEnvelopeYp(opt.envelopeYp);
  SetEnvelopeT(opt.envelopeT);
  SetEnvelopeE(opt.envelopeE);
  return; 
}

void BDSBunchCircle::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight) {
  x0 = X0 * CLHEP::m;
  y0 = Y0 * CLHEP::m;
  z0 = Z0 * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  z0 = Z0 * CLHEP::m + (T0 - envelopeT * (1.-2.*FlatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  
  if(envelopeX !=0) x0  += envelopeX * (1-2*FlatGen->shoot()) * CLHEP::m;
  if(envelopeY !=0) y0  += envelopeY * (1-2*FlatGen->shoot()) * CLHEP::m;
  if(envelopeXp !=0) xp += envelopeXp * (1-2*FlatGen->shoot()) * CLHEP::rad;
  if(envelopeYp !=0) yp += envelopeYp * (1-2*FlatGen->shoot()) * CLHEP::rad;
  
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp = sqrt(1.-xp*xp -yp*yp);
  t = 0;
  E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * (1 + sigmaE * (1-2*FlatGen->shoot()));

  weight = 1.0;
  return; 
}
