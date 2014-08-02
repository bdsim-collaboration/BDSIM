#include "BDSBunchGaussian.hh"
#include <string.h>

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

  // Fill means 
  meansGM[0] = X0;
  meansGM[1] = Xp0;
  meansGM[2] = Y0;
  meansGM[3] = Yp0;
  meansGM[4] = T0;
  meansGM[5] = 1;

  // Fill sigmas 
  sigmaGM[0][0] = sigmaX; 
  sigmaGM[1][1] = sigmaXp; 
  sigmaGM[2][2] = sigmaY; 
  sigmaGM[3][3] = sigmaYp; 

  // Create multi dim gaussian generator
  GaussMultiGen = new CLHEP::RandMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM); 
}

BDSBunchGaussian::BDSBunchGaussian(G4double *sigma, 
				   G4double X0In,     G4double Y0In,  G4double Z0In,  G4double T0In,
				   G4double Xp0In,    G4double Yp0In, G4double Zp0In, 
				   G4double sigmaTIn, G4double sigmaEIn) :
  BDSBunchInterface(X0In,Y0In, Z0In, T0In, Xp0In, Yp0In, Zp0In, sigmaTIn, sigmaEIn)
{
  meansGM = CLHEP::HepVector(6);
  sigmaGM = CLHEP::HepSymMatrix(6);

  // Fill means 
  meansGM[0] = X0;
  meansGM[1] = Xp0;
  meansGM[2] = Y0;
  meansGM[3] = Yp0;
  meansGM[4] = T0;
  meansGM[5] = 1;

  // Fill sigmas 
  sigmaGM[0][0] = sigma[0]; 
  sigmaGM[0][1] = sigma[1];
  sigmaGM[0][2] = sigma[2];
  sigmaGM[0][3] = sigma[3];
  sigmaGM[0][4] = sigma[4];
  sigmaGM[0][5] = sigma[5];  
  sigmaGM[1][1] = sigma[6];
  sigmaGM[1][2] = sigma[7];
  sigmaGM[1][3] = sigma[8];
  sigmaGM[1][4] = sigma[9];
  sigmaGM[1][5] = sigma[10];  
  sigmaGM[2][2] = sigma[11];
  sigmaGM[2][3] = sigma[12];
  sigmaGM[2][4] = sigma[13];
  sigmaGM[2][5] = sigma[14];  
  sigmaGM[3][3] = sigma[15];
  sigmaGM[3][4] = sigma[16];
  sigmaGM[3][5] = sigma[17];  
  sigmaGM[4][4] = sigma[18];
  sigmaGM[4][5] = sigma[19];  
  sigmaGM[5][5] = sigma[20];

  // Create multi dim gaussian
  if(GaussMultiGen != NULL) 
    delete GaussMultiGen;
  GaussMultiGen = new CLHEP::RandMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM); 
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

  BDSBunchInterface::SetOptions(opt);
  
  meansGM[0]    = X0;
  meansGM[1]    = Xp0;
  meansGM[2]    = Y0;
  meansGM[3]    = Yp0;
  meansGM[4]    = T0;
  meansGM[5]    = 1;
      
  if(strcmp(opt.distribType.data(),"gauss") == 0) {
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
  }
  else if (strcmp(opt.distribType.data(),"gaussmatrix") == 0) 
  {    
    sigmaGM[0][0] = opt.sigmaX; 
    sigmaGM[1][1] = opt.sigmaXp; 
    sigmaGM[2][2] = opt.sigmaY; 
    sigmaGM[3][3] = opt.sigmaYp;       
  }

  if(GaussMultiGen != NULL) delete GaussMultiGen;
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
