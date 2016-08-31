#include "BDSBunchHalo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "parser/options.h"

#include "globals.hh" // geant4 types / globals

#include "CLHEP/Random/RandFlat.h"

#include <cmath>

BDSBunchHalo::BDSBunchHalo():
  betaX(0.0), betaY(0.0),
  alphaX(0.0), alphaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  envelopeX(0.0), envelopeY(0.0),
  envelopeXp(0.0), envelopeYp(0.0),
  envelopeEmitX(0.0), envelopeEmitY(0.0),
  envelopeCollMinX(0.0), envelopeCollMaxX(0.0),
  envelopeCollMinXp(0.0), envelopeCollMaxXp(0.0),
  envelopeCollMinY(0.0), envelopeCollMaxY(0.0),
  envelopeCollMinYp(0.0), envelopeCollMaxYp(0.0),
  twoLobeX(false), twoLobeY(false),
  xMinDist(0.0), xMaxDist(0.0), xMinMaxRatio(0.0),
  yMinDist(0.0), yMaxDist(0.0), yMinMaxRatio(0.0)
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
  SetEmitX(opt.haloEmitX);
  SetEmitY(opt.haloEmitY);  
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;  
  SetEnvelopeX(opt.envelopeX); 
  SetEnvelopeY(opt.envelopeY);
  SetEnvelopeXp(opt.envelopeXp);
  SetEnvelopeYp(opt.envelopeYp);
  SetEnvelopeEmitX(opt.haloEnvelopeEmitX);
  SetEnvelopeEmitY(opt.haloEnvelopeEmitY);
  SetEnvelopeCollMinX(opt.haloEnvelopeCollMinX);
  SetEnvelopeCollMaxX(opt.haloEnvelopeCollMaxX);
  SetEnvelopeCollMinXp(opt.haloEnvelopeCollMinXp);
  SetEnvelopeCollMaxXp(opt.haloEnvelopeCollMaxXp);
  SetEnvelopeCollMinY(opt.haloEnvelopeCollMinY);
  SetEnvelopeCollMaxY(opt.haloEnvelopeCollMaxY);
  SetEnvelopeCollMinYp(opt.haloEnvelopeCollMinYp);
  SetEnvelopeCollMaxYp(opt.haloEnvelopeCollMaxYp);

  SetWeightParameter(opt.haloPSWeightParameter);
  SetWeightFunction(opt.haloPSWeightFunction);
  
  if (BDS::IsFinite(envelopeCollMinX) ||  BDS::IsFinite(envelopeCollMaxX))
  {
    G4double distCutOutX = std::abs(envelopeCollMinX) + std::abs(envelopeCollMaxX);
    if (distCutOutX > 0.5*envelopeX)
	  {
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "using two lobe method of generation in X." << G4endl;
#endif
	    twoLobeX = true;
	    xMinDist = envelopeX - std::abs(envelopeCollMinX);
	    xMaxDist = envelopeX - std::abs(envelopeCollMaxX);
	    xMinMaxRatio = xMinDist / (xMinDist + xMaxDist);
    }
  }
  
  if (BDS::IsFinite(envelopeCollMinY) ||  BDS::IsFinite(envelopeCollMaxY))
  {
    G4double distCutOutY = std::abs(envelopeCollMinY) + std::abs(envelopeCollMaxY);
    if (distCutOutY > 0.5*envelopeY)
	  {
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "using two lobe method of generation in Y." << G4endl;
#endif
	    twoLobeY = true;
	    yMinDist = envelopeY - std::abs(envelopeCollMinY);
	    yMaxDist = envelopeY - std::abs(envelopeCollMaxY);
	    yMinMaxRatio = yMinDist / (yMinDist + yMaxDist);
	  }
  }
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

  while(true)
  {
    // Flat 2x2d phase space

    // Generate x,y spatial in optionally two lobes for increased efficiency.
    G4double dx = 0;
    if (twoLobeX)
    {
	    // choose with random uniform distribution each side proportioned by area of each lobe
	    G4bool positiveSide = FlatGen->shoot() > xMinMaxRatio;
	    if (positiveSide)
	    {dx = envelopeCollMaxX + xMaxDist*FlatGen->shoot();}
	    else
	    {dx = -(envelopeCollMaxX + xMinDist*FlatGen->shoot());}
    }
    else
    {dx = envelopeX  * (1 - 2 * FlatGen->shoot());}

    G4double dy = 0;
    if (twoLobeY)
    {

	    // choose with random uniform distribution each side proportioned by area of each lobe
	    G4bool positiveSide = FlatGen->shoot() > yMinMaxRatio;
	    if (positiveSide)
	    {dy = envelopeCollMaxY + yMaxDist*FlatGen->shoot();}
	    else
	    {dy = -(envelopeCollMaxY + yMinDist*FlatGen->shoot());}
    }
    else
    {dy = envelopeY  * (1 - 2 * FlatGen->shoot());}
    
    G4double dxp = envelopeXp * (1 - 2 * FlatGen->shoot());
    G4double dyp = envelopeYp * (1 - 2 * FlatGen->shoot());

    // compute single particle emittance 
    double emitXSp = gammaX * pow(dx, 2) + 2. * alphaX * dx * dxp + betaX * pow(dxp, 2);
    double emitYSp = gammaY * pow(dy, 2) + 2. * alphaY * dy * dyp + betaY * pow(dyp, 2);

    // #ifdef BDSDEBUG
    //    G4cout << __METHOD_NAME__ << "phase space> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
    //    G4cout << __METHOD_NAME__ << "Xcollimators> "  << envelopeCollMinX << " " << envelopeCollMaxX << " " << envelopeCollMinXp << " " << envelopeCollMaxXp << G4endl;
    //    G4cout << __METHOD_NAME__ << "Ycollimators> "  << envelopeCollMinY << " " << envelopeCollMaxY << " " << envelopeCollMinYp << " " << envelopeCollMaxYp << G4endl;
    //    G4cout << __METHOD_NAME__ << "emittance> " << emitX << " " << emitXSp << " " << envelopeEmitX << " "
    //                                               << emitY << " " << emitYSp << " " << envelopeEmitY << G4endl;
    // #endif

    // check if particle is within normal beam core, if so continue generation
    if ((emitXSp < emitX || emitYSp < emitY) || (emitXSp > envelopeEmitX || emitYSp > envelopeEmitY)  ||
        ((dx  > envelopeCollMinX)  && (dx < envelopeCollMaxX)) ||
        ((dy  > envelopeCollMinY)  && (dy < envelopeCollMaxY)))
    {
      continue;
    }
    else
    {
      // determine weight, initialise 1 so always passes
      double wx = 1.0;
      double wy = 1.0;
      if (weightFunction == "flat" || weightFunction == "")
      {
        wx = 1.0;
        wy = 1.0;
      }
      else if (weightFunction == "oneoverr")
      {
        wx = pow(fabs(emitXSp) / emitX, weightParameter);
        wy = pow(fabs(emitYSp) / emitY, weightParameter);
      }
      else if (weightFunction == "exp")
      {
        wx = exp(-(emitXSp - emitX) / (emitX * weightParameter));
        wy = exp(-(emitYSp - emitY) / (emitY * weightParameter));
      }

 #ifdef BDSDEBUG
       G4cout << __METHOD_NAME__ << emitXSp/emitX << " " << emitYSp/emitY << " " << wx << " " << wy << G4endl;
 #endif
      // reject
      if(FlatGen->shoot() > wx && FlatGen->shoot() > wy)
	    {continue;}

      // add to reference orbit 
      x0 += dx * CLHEP::m;
      y0 += dy * CLHEP::m;
      xp += dxp * CLHEP::rad;
      yp += dyp * CLHEP::rad;

      zp = CalculateZp(xp, yp, Zp0);
      t = 0 * CLHEP::s;
      E = BDSGlobalConstants::Instance()->ParticleTotalEnergy();

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "selected> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
#endif

      weight = 1.0;
      return;
    }
  }
}
