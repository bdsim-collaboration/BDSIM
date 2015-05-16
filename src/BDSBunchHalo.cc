#include "BDSBunchHalo.hh"
#include "BDSDebug.hh"

BDSBunchHalo::BDSBunchHalo() : BDSBunchInterface(), betaY(0.0), alphaX(0.0), alphaY(0.0), emitX(0.0), emitY(0.0), gammaX(0.0), gammaY(0.0), envelopeX(0.0), envelopeY(0.0), envelopeXp(0.0), envelopeYp(0.0) {
  
}


BDSBunchHalo::BDSBunchHalo(G4double betaXIn,      G4double betaYIn, 
			   G4double alphaXIn,     G4double alphaYIn,
			   G4double emitXIn,      G4double emitYIn,
			   G4double envelopeXIn,  G4double envelopeYIn,
			   G4double envelopeXpIn, G4double envelopeYpIn,
			   G4double X0In,         G4double Y0In,       G4double Z0In,   G4double T0In, 
			   G4double Xp0In,        G4double Yp0In,      G4double Zp0In,			     
			   G4double sigmaTIn,     G4double sigmaEIn) : BDSBunchInterface(X0In,Y0In,Z0In,T0In,Xp0In,Yp0In,Zp0In,sigmaTIn,sigmaEIn), betaX(betaXIn), betaY(betaYIn), alphaX(alphaXIn), alphaY(alphaYIn), emitX(emitXIn), emitY(emitYIn),   envelopeX(envelopeXIn), envelopeY(envelopeYIn), envelopeXp(envelopeXpIn), envelopeYp(envelopeYpIn) 
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
}

BDSBunchHalo::~BDSBunchHalo() 
{ 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  delete FlatGen; 
}

void  BDSBunchHalo::SetOptions(struct Options &opt) { 

  BDSBunchInterface::SetOptions(opt);
  SetBetaX(opt.betx);
  SetBetaY(opt.bety);
  SetAlphaX(opt.alfx);
  SetAlphaY(opt.alfy);
  SetEmitX(opt.emitx);
  SetEmitY(opt.emity);  
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;  
  SetEnvelopeX(opt.envelopeX); 
  SetEnvelopeY(opt.envelopeY);
  SetEnvelopeXp(opt.envelopeXp);
  SetEnvelopeYp(opt.envelopeYp); 
}

void BDSBunchHalo::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				   G4double& xp, G4double& yp, G4double& zp,
				   G4double& t , G4double&  E, G4double& weight) {
  return;
}
