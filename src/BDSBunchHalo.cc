#include "BDSBunchHalo.hh"
#include "BDSDebug.hh"

BDSBunchHalo::BDSBunchHalo():
  betaX(0.0), betaY(0.0),
  alphaX(0.0), alphaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  envelopeX(0.0), envelopeY(0.0),
  envelopeXp(0.0), envelopeYp(0.0)
{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
  weightParameter=1.0;
}

BDSBunchHalo::~BDSBunchHalo() 
{
  delete FlatGen; 
}

void  BDSBunchHalo::SetOptions(const GMAD::Options& opt)
{
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
  SetWeightParameter(opt.haloPSWeightParameter);
  SetWeightFunction(opt.haloPSWeightFunction);
}

void BDSBunchHalo::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				   G4double& xp, G4double& yp, G4double& zp,
				   G4double& t , G4double&  E, G4double& weight)
{
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
    double emitYSp = gammaY*pow(dy,2) + 2.*alphaY*dy*dyp + betaY*pow(dyp,2);
    
// #ifdef BDSDEBUG
//     G4cout << __METHOD_NAME__ << "phase space> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
//     G4cout << __METHOD_NAME__ << "emittance> " << emitXSp << " " << emitX << " " << emitYSp << " " << emitY << G4endl;
// #endif

    // check if particle is within normal beam core, if so continue generation
    if (emitXSp < emitX || emitYSp <emitY) { 
// #ifdef BDSDEBUG
//       G4cout << __METHOD_NAME__ << "continue> " << G4endl;
// #endif
      continue;
    } 
    else {
      // determine weight, initialise 1 so always passes
      double wx = 1.0;
      double wy = 1.0;
      if(weightFunction == "flat" || weightFunction == "") { 
	wx = 1.0;
	wy = 1.0;
      }
      else if (weightFunction == "oneoverr") { 
	wx = pow(emitX/fabs(emitXSp),weightParameter);
	wy = pow(emitY/fabs(emitYSp),weightParameter);	
      }
      else if (weightFunction == "exp") {
	wx = exp(-(emitXSp-emitX)/(emitX*weightParameter));
	wy = exp(-(emitYSp-emitY)/(emitY*weightParameter));
      }
      
// #ifdef BDSDEBUG
//       G4cout << __METHOD_NAME__ << emitXSp/emitX << " " << emitYSp/emitY << " " << wx << " " << wy << G4endl;
// #endif
      // reject
      if(FlatGen->shoot() > wx && FlatGen->shoot() > wy) 
	continue;

      // add to reference orbit 
      x0 += dx;
      y0 += dy;
      xp += dxp;
      yp += dyp;
      
      zp = CalculateZp(xp,yp,Zp0);
      t = 0 * CLHEP::s;
      E = BDSGlobalConstants::Instance()->GetParticleTotalEnergy();

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "selected> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
#endif
     
      weight = 1.0;
      return;
    }
  }
}
