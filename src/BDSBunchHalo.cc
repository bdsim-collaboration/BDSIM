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

  // Central orbit 
  x0 = X0  * CLHEP::m;
  y0 = Y0  * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  z0 = Z0  * CLHEP::m; 

  //  z0 += (T0 - envelopeT * (1.-2.*FlatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  z0 += 0;

  while(true) {
    // Flat 2x2d phase space
    G4double dx  = envelopeX  * (1-2*FlatGen->shoot()) * CLHEP::m;
    G4double dy  = envelopeY  * (1-2*FlatGen->shoot()) * CLHEP::m;
    G4double dxp = envelopeXp * (1-2*FlatGen->shoot()) * CLHEP::rad;
    G4double dyp = envelopeYp * (1-2*FlatGen->shoot()) * CLHEP::rad;

    // compute single particle emittance 
    double emitXSp = gammaX*pow(dx,2) + 2.*alphaX*dx*dxp + betaX*pow(dxp,2);
    double emitYSp = gammaY*pow(dy,2) + 2.*alphaY*dy*dyp + betaX*pow(dyp,2);
    
    std::cout << "emittance> " << emitXSp << " " << emitX << " " << emitYSp << " " << emitY << std::endl;

    // check if particle is within normal beam core, if so continue generation
    if (emitXSp < emitX || emitYSp <emitY) { 
      std::cout << "continue> " << std::endl;
      continue;
    }    
    else {
      // determine weight

      // add to reference orbit 
      x0 += dx;
      y0 += dy;
      xp += dxp;
      yp += dyp;
      
      zp = CalculateZp(xp,yp,Zp0);
      t = 0 * CLHEP::s;
      E = BDSGlobalConstants::Instance()->GetParticleKineticEnergy();

      G4cout << "selected> " << dx << " " << dy << " " << dxp << " " << dyp << std::endl;
      
      weight = 1.0;
      return;
    }
  }
}
