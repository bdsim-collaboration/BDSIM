#include "BDSBunchHalo.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include "parser/options.h"

#include "globals.hh" // geant4 types / globals

#include "Randomize.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchHalo::BDSBunchHalo():
  alphaX(0.0), alphaY(0.0),
  betaX(0.0), betaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  sigmaX(0.0), sigmaY(0.0),
  haloNSigmaXInner(0.0), haloNSigmaXOuter(0.0),
  haloNSigmaYInner(0.0), haloNSigmaYOuter(0.0),
  haloXCutInner(0.0), 
  haloYCutInner(0.0),
  haloPSWeightParameter(0.0),
  haloNSigmaXpOuter(0.0),
  haloNSigmaYpOuter(0.0),
  weightFunction(""),
  emitInnerX(0.0), emitInnerY(0.0),
  emitOuterX(0.0), emitOuterY(0.0),
  xMax(0.0), yMax(0.0),
  xpMax(0.0), ypMax(0.0),
  twoLobeX(false), twoLobeY(false),
  xMinDist(0.0), xMinMaxRatio(0.0),
  yMinDist(0.0), yMinMaxRatio(0.0)


{
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
  //weightParameter=1.0;
}

BDSBunchHalo::~BDSBunchHalo() 
{
  delete FlatGen; 
}

void  BDSBunchHalo::SetOptions(const GMAD::Options& opt,
			       G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(opt, beamlineTransformIn);
  alphaX                = G4double(opt.alfx);
  alphaY                = G4double(opt.alfy);
  betaX                 = G4double(opt.betx);
  betaY                 = G4double(opt.bety);
  emitX                 = G4double(opt.emitx);
  emitY                 = G4double(opt.emity);
  gammaX                = (1.0+alphaX*alphaX)/betaX;
  gammaY                = (1.0+alphaY*alphaY)/betaY;
  sigmaX                = sqrt(emitX * betaX);
  sigmaY                = sqrt(emitY * betaY);    
  haloNSigmaXInner      = G4double(opt.haloNSigmaXInner);
  haloNSigmaXOuter      = G4double(opt.haloNSigmaXOuter);
  haloNSigmaYInner      = G4double(opt.haloNSigmaYInner);
  haloNSigmaYOuter      = G4double(opt.haloNSigmaYOuter);
  haloXCutInner         = G4double(opt.haloXCutInner);
  haloYCutInner         = G4double(opt.haloYCutInner);  
  haloPSWeightParameter = G4double(opt.haloPSWeightParameter);
  weightFunction = G4String(opt.haloPSWeightFunction);  

  haloNSigmaXpOuter     = std::sqrt(gammaX * emitX);
  haloNSigmaYpOuter     = std::sqrt(gammaY * emitY);   



  emitInnerX = std::pow(haloNSigmaXInner, 2) * emitX;
  emitInnerY = std::pow(haloNSigmaYInner, 2) * emitY;
  emitOuterX = std::pow(haloNSigmaXOuter, 2) * emitX;
  emitOuterY = std::pow(haloNSigmaYOuter, 2) * emitY;  

  xMax  = haloNSigmaXOuter * sigmaX;
  xMin  = haloNSigmaXInner * sigmaX;
  yMax  = haloNSigmaYOuter * sigmaY;
  yMin  = haloNSigmaYInner * sigmaY;
  xpMax = std::sqrt(std::pow(haloNSigmaXOuter, 2) * emitX * gammaX);
  ypMax = std::sqrt(std::pow(haloNSigmaYOuter, 2) * emitY * gammaY);    

  if (BDS::IsFinite(haloXCutInner))
  {
    //G4double distCutOutX = std::abs(haloXCutInner * sigmaX);
    
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "using two lobe method of generation in X." << G4endl;
#endif
    twoLobeX = true;
    xMinDist = xMax - std::abs(haloXCutInner * sigmaX);
    xMinMaxRatio = xMinDist / std::abs(xMax);
    
  }
  
  if (BDS::IsFinite(haloYCutInner))
  {
    //G4double distCutOutY = std::abs(haloYCutInner * sigmaY);
    
    
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "using two lobe method of generation in Y." << G4endl;
#endif
    twoLobeY = true;
    yMinDist = yMax - std::abs(haloYCutInner * sigmaY);
    yMinMaxRatio = yMinDist / std::abs(yMax);
    
  }
  CheckParameters();
  
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
	  {dx = xMax + xMaxDist*FlatGen->shoot();}
	else
	  {dx = -(xMax + xMinDist*FlatGen->shoot());}
      }
    else
      {dx = xMax  * (1 - 2 * FlatGen->shoot());}
    
    G4double dy = 0;
    if (twoLobeY)
      {
	
	// choose with random uniform distribution each side proportioned by area of each lobe
	G4bool positiveSide = FlatGen->shoot() > yMinMaxRatio;
	if (positiveSide)
	  {dy = yMax + yMaxDist*FlatGen->shoot();}
	else
	  {dy = -(yMax + yMinDist*FlatGen->shoot());}
      }
    else
      {dy = yMax  * (1 - 2 * FlatGen->shoot());}

    
    G4double dxp = xpMax * (1 - 2 * FlatGen->shoot());
    G4double dyp = ypMax * (1 - 2 * FlatGen->shoot());
    

    // compute single particle emittance 
    double emitXSp = gammaX * std::pow(std::abs(dx), 2) + 2. * alphaX * dx * dxp + betaX * std::pow(std::abs(dxp), 2);
    double emitYSp = gammaY * std::pow(std::abs(dy), 2) + 2. * alphaY * dy * dyp + betaY * std::pow(std::abs(dyp), 2);

    // #ifdef BDSDEBUG
    //    G4cout << __METHOD_NAME__ << "phase space> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
    //    G4cout << __METHOD_NAME__ << "Xcollimators> "  << envelopeCollMinX << " " << envelopeCollMaxX << " " << envelopeCollMinXp << " " << envelopeCollMaxXp << G4endl;
    //    G4cout << __METHOD_NAME__ << "Ycollimators> "  << envelopeCollMinY << " " << envelopeCollMaxY << " " << envelopeCollMinYp << " " << envelopeCollMaxYp << G4endl;
    //    G4cout << __METHOD_NAME__ << "emittance> " << emitX << " " << emitXSp << " " << envelopeEmitX << " "
    //                                               << emitY << " " << emitYSp << " " << envelopeEmitY << G4endl;
    // #endif

    // check if particle is within normal beam core, if so continue generation
      // also check if particle is within the desired cut.
    if ((std::abs(emitXSp) < emitInnerX || std::abs(emitYSp) < emitInnerY) ||
	(std::abs(emitXSp) > emitOuterX || std::abs(emitYSp) > emitOuterY)  ||
        (std::abs(dx)  < (haloXCutInner * sigmaX)) ||
	(std::abs(dy)  < (haloYCutInner * sigmaY)) )
    {
      continue;
    }
    else
    {
      // determine weight, initialise 1 so always passes
      double wx = 0.0;
      double wy = 0.0;
      if (weightFunction == "flat" || weightFunction == "")
      {
        wx = 1.0;
        wy = 1.0;
      }
      else if (weightFunction == "oneoverr")
	{
	  //abs because power of double - must be positive
	  wx = std::pow(std::abs(emitInnerX / emitXSp), haloPSWeightParameter);
	  wy = std::pow(std::abs(emitInnerY / emitYSp), haloPSWeightParameter);
	}
      else if (weightFunction == "oneoverrsqrd")
	{
	  //abs because power of double - must be positive
	  double eXsqrd = std::pow(std::abs(emitXSp), 2);
	  double eYsqrd = std::pow(std::abs(emitYSp), 2);
	  double eXInsq = std::pow(std::abs(emitInnerX), 2);
	  double eYInsq = std::pow(std::abs(emitInnerY), 2);
	  wx = std::pow(std::abs(eXInsq / eXsqrd), haloPSWeightParameter);
	  wy = std::pow(std::abs(eYInsq / eYsqrd), haloPSWeightParameter);
	}
      else if (weightFunction == "exp")
      {
        wx = std::exp(-(emitXSp * haloPSWeightParameter) / (emitInnerX));
        wy = std::exp(-(emitYSp * haloPSWeightParameter) / (emitInnerY));
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

      ApplyTransform(x0,y0,z0,xp,yp,zp);
      
      t = 0 * CLHEP::s;
      E = E0 * CLHEP::GeV;

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "selected> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
#endif

      weight = 1.0;
      return;
    }
  }
}

void BDSBunchHalo::CheckParameters()
{
  std::vector<G4String> weightFunctions = {"", "flat","oneoverr", "oneoverrsqrd", "exp"};
  auto search = std::find(weightFunctions.begin(), weightFunctions.end(), weightFunction);
  if (search == weightFunctions.end())
    {
      G4cerr << __METHOD_END__ << "invalid haloPSWeightFunction \"" << weightFunction << "\"" << G4endl;
      G4cout << "Available weight functions are:" << G4endl;
      for (const auto& w : weightFunctions)
        {G4cout << w << G4endl;}
      exit(1);
    }
  
  if (haloNSigmaXInner == 0)
    {G4cerr << __METHOD_NAME__ << "haloNSigmaXInner cannot be zero" << G4endl; exit(1);}

  if (haloNSigmaYInner == 0)
    {G4cerr << __METHOD_NAME__ << "haloYSigmaXInner cannot be zero" << G4endl; exit(1);}
  

}
