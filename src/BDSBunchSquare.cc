#include "BDSBunchSquare.hh"

BDSBunchSquare::BDSBunchSquare() :
  BDSBunchInterface(), envelopeX(0.0), envelopeY(0.0), 
  envelopeXp(0.0), envelopeYp(0.0), envelopeT(0.0), envelopeE(0.0)
{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchSquare::BDSBunchSquare(G4double envelopeXIn,  G4double envelopeYIn,
			       G4double envelopeXpIn, G4double envelopeYpIn,
			       G4double envelopeTIn,  G4double envelopeEIn,
			       G4double X0In,         G4double Y0In,         G4double Z0In,   G4double T0In, 
			       G4double Xp0In,        G4double Yp0In,        G4double Zp0In) :  
  BDSBunchInterface(X0In,Y0In,Z0In,T0In,Xp0In,Yp0In,Zp0In,0.0,0.0), 
  envelopeX(envelopeXIn), envelopeY(envelopeYIn), 
  envelopeXp(envelopeXpIn), envelopeYp(envelopeYpIn), 
  envelopeT(envelopeTIn), envelopeE(envelopeEIn)
{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchSquare::~BDSBunchSquare() 
{
  delete FlatGen;
}

void BDSBunchSquare::SetOptions(struct Options &opt) {
  SBDSBunchInterface::SetOptions(opt)
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
  E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * (1 + envelopeE * (1-2*FlatGen->shoot()));

  weight = 1.0;
  return; 
}
