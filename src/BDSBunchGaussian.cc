#include "BDSBunchGaussian.hh"

BDSBunchGaussian::BDSBunchGaussian() : BDSBunchInterface() {
  meansGM = CLHEP::HepVector(6);
  sigmaGM = CLHEP::HepSymMatrix(6);
  GaussMultiGen = NULL;
}

BDSBunchGaussian::BDSBunchGaussian(G4double sigmaXIn, G4double sigmaYIn, G4double sigmaXpIn, G4double sigmaYpIn, 
				   G4double X0In,     G4double Y0In,     G4double Z0In,      G4double T0In,
				   G4double Xp0In,    G4double Yp0In,    G4double Zp0In, 
				   G4double sigmaTIn, G4double sigmaEIn) :
  BDSBunchInterface(X0In,Y0In, Z0In, T0In, Xp0In, Yp0In, Zp0In, sigmaTIn, sigmaEIn), 
  sigmaX(sigmaXIn), sigmaY(sigmaYIn), sigmaXp(sigmaXpIn), sigmaYp(sigmaYpIn)
{
  meansGM = CLHEP::HepVector(6);
  sigmaGM = CLHEP::HepSymMatrix(6);
  GaussMultiGen = NULL;
}

BDSBunchGaussian::BDSBunchGaussian(G4double *sigma, 
				   G4double X0In,     G4double Y0In,  G4double Z0In,  G4double T0In,
				   G4double Xp0In,    G4double Yp0In, G4double Zp0In, 
				   G4double sigmaTIn, G4double sigmaEIn) {
  meansGM = CLHEP::HepVector(6);
  sigmaGM = CLHEP::HepSymMatrix(6);
  GaussMultiGen = NULL;
}

BDSBunchGaussian::~BDSBunchGaussian() {
  delete GaussMultiGen;
}

void BDSBunchGaussian::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  
  SetSigmaX(opt.sigmaX); 
  SetSigmaY(opt.sigmaY);
  SetSigmaXp(opt.sigmaXp);
  SetSigmaYp(opt.sigmaYp);

  meansGM[0] = X0;
  meansGM[1] = Xp0;
  meansGM[2] = Y0;
  meansGM[3] = Yp0;
  meansGM[4] = T0;
  meansGM[5] = 1;
      
  sigmaGM[0][0] = opt.sigma11; 
  sigmaGM[0][1] = opt.sigma12;
  sigmaGM[0][2] = opt.sigma13;
  sigmaGM[0][3] = opt.sigma14;
  sigmaGM[0][4] = opt.sigma15;
  sigmaGM[0][5] = opt.sigma16;  
  sigmaGM[1][1] = opt.sigma22;
  sigmaGM[1][2] = opt.sigma23;
  sigmaGM[1][3] = opt.sigma24;
  sigmaGM[1][4] = opt.sigma25;
  sigmaGM[1][5] = opt.sigma26;  
  sigmaGM[2][2] = opt.sigma33;
  sigmaGM[2][3] = opt.sigma34;
  sigmaGM[2][4] = opt.sigma35;
  sigmaGM[2][5] = opt.sigma36;  
  sigmaGM[3][3] = opt.sigma44;
  sigmaGM[3][4] = opt.sigma45;
  sigmaGM[3][5] = opt.sigma46;  
  sigmaGM[4][4] = opt.sigma55;
  sigmaGM[4][5] = opt.sigma56;  
  sigmaGM[5][5] = opt.sigma66;

  GaussMultiGen = new CLHEP::RandMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM); 

  return;
}

void BDSBunchGaussian::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight) {
  CLHEP::HepVector v = GaussMultiGen->fire();
  x0 = v[0]*CLHEP::m;
  xp = v[1]*CLHEP::rad;
  y0 = v[2]*CLHEP::m;
  yp = v[3]*CLHEP::rad;
  t  = v[4];
  z0 = Z0*CLHEP::m + t*CLHEP::c_light;
  E  = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * v[5];
  
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp =  sqrt(1.-xp*xp -yp*yp);

  weight = 1.0;
  return;
}
