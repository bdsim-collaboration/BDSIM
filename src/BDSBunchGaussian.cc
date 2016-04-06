#include "BDSBunchGaussian.hh"
#include <cstring>
#include "BDSDebug.hh"

BDSBunchGaussian::BDSBunchGaussian(): 
  BDSBunchInterface(),
  sigmaX(0.0),sigmaY(0.0),sigmaXp(0.0),sigmaYp(0.0)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  meansGM = CLHEP::HepVector(6);
  sigmaGM = CLHEP::HepSymMatrix(6);
  GaussMultiGen = nullptr;
}

BDSBunchGaussian::~BDSBunchGaussian()
{
  delete GaussMultiGen;
}

void BDSBunchGaussian::SetOptions(const GMAD::Options& opt)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  
  SetSigmaX(opt.sigmaX); 
  SetSigmaY(opt.sigmaY);
  SetSigmaXp(opt.sigmaXp);
  SetSigmaYp(opt.sigmaYp);
  
  meansGM[0]    = X0;
  meansGM[1]    = Xp0;
  meansGM[2]    = Y0;
  meansGM[3]    = Yp0;
  meansGM[4]    = T0;
  meansGM[5]    = 1;

  if(strcmp(opt.distribType.data(),"gaussmatrix") == 0) {
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
  else if (strcmp(opt.distribType.data(),"gauss") == 0) 
  {    
    sigmaGM[0][0] = pow(opt.sigmaX,2); 
    sigmaGM[1][1] = pow(opt.sigmaXp,2); 
    sigmaGM[2][2] = pow(opt.sigmaY,2); 
    sigmaGM[3][3] = pow(opt.sigmaYp,2);       
    sigmaGM[4][4] = pow(opt.sigmaT,2); 
    sigmaGM[5][5] = pow(opt.sigmaE,2);
  }
#ifdef BDSDEBUG
  G4cout << "sigmaGM" << sigmaGM << G4endl;
#endif
  delete GaussMultiGen;
  GaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);
  return;
}

void BDSBunchGaussian::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				       G4double& xp, G4double& yp, G4double& zp,
				       G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  CLHEP::HepVector v = GaussMultiGen->fire();
#ifdef BDSDEBUG 
  G4cout << "HEPVECTOR" << v << G4endl;
#endif
  x0 = v[0] * CLHEP::m;
  xp = v[1] * CLHEP::rad;
  y0 = v[2] * CLHEP::m;
  yp = v[3] * CLHEP::rad;
  t  = v[4] * CLHEP::s;
  zp = 0.0  * CLHEP::rad;
  z0 = Z0*CLHEP::m + t*CLHEP::c_light;
  E  = BDSGlobalConstants::Instance()->ParticleTotalEnergy() * v[5];   //Geant4 require KE, not total.
  zp = CalculateZp(xp,yp,Zp0);

  weight = 1.0;
  return;
}
