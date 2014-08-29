#include "BDSBunchComposite.hh"

BDSBunchComposite::BDSBunchComposite() {
  xBunch = NULL;
  yBunch = NULL;
}

BDSBunchComposite::~BDSBunchComposite() {
  if(xBunch) 
    delete xBunch;
  if(yBunch) 
    delete yBunch;

}

void BDSBunchComposite::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  

  if (opt.xDistribType == "reference") 
    xBunch = new BDSBunchInterface();
  else if(opt.xDistribType == "gauss" || opt.xDistribType == "gaussmatrix") 
    xBunch = new BDSBunchGaussian(); 
  else if(opt.xDistribType == "square") 
    xBunch = new BDSBunchSquare();
  else if(opt.xDistribType == "circle") 
    xBunch = new BDSBunchCircle();
  else if(opt.xDistribType == "ring") 
    xBunch = new BDSBunchRing();
  else if(opt.xDistribType == "eshell") 
    xBunch = new BDSBunchEShell();
  else if(opt.xDistribType == "gausstwiss") 
    xBunch = new BDSBunchTwiss();
  else {
    G4cerr << "xDistribType not found " << opt.xDistribType << G4endl;
    exit(1);
  }

  if (opt.yDistribType == "reference") 
    yBunch = new BDSBunchInterface();
  else if(opt.yDistribType == "gauss" || opt.yDistribType == "gaussmatrix") 
    yBunch = new BDSBunchGaussian(); 
  else if(opt.yDistribType == "square") 
    yBunch = new BDSBunchSquare();
  else if(opt.yDistribType == "circle") 
    yBunch = new BDSBunchCircle();
  else if(opt.yDistribType == "ring") 
    yBunch = new BDSBunchRing();
  else if(opt.yDistribType == "eshell") 
    yBunch = new BDSBunchEShell();
  else if(opt.yDistribType == "gausstwiss") 
    yBunch = new BDSBunchTwiss();
  else {
    G4cerr << "yDistribType not found " << opt.yDistribType << G4endl;
    exit(1);
  }

 
  xBunch->SetOptions(opt);
  yBunch->SetOptions(opt);
  
}

void BDSBunchComposite::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
					G4double& xp, G4double& yp, G4double& zp,
					G4double& t , G4double&  E, G4double& weight) { 
  G4double xx0, xy0, xz0, xxp, xyp, xzp, xt, xE, xWeight;
  G4double yx0, yy0, yz0, yxp, yyp, yzp, yt, yE, yWeight;
  
  xBunch->GetNextParticle(xx0, xy0, xz0, xxp, xyp, xzp, xt, xE, xWeight);
  yBunch->GetNextParticle(yx0, yy0, yz0, yxp, yyp, yzp, yt, yE, yWeight);

  x0 = xx0;
  xp = xxp;
  y0 = yy0;
  yp = yyp;
  z0 = xz0;
  zp = xzp; // Is this correct?
  t  = xt;
  E  = xE; 
  weight = xWeight;

  return;
}
