#include "BDSBunchGaussian.hh"
#include "BDSDebug.hh" 
#include <string.h>

/// helper method
namespace {
  /// check if matrix is positive definite
  bool isPositiveDefinite(CLHEP::HepSymMatrix& S)
  {
    CLHEP::HepSymMatrix temp (S); // Since diagonalize does not take a const s
                                  // we have to copy S.
    CLHEP::HepMatrix U = diagonalize ( &temp ); // S = U Sdiag U.T()
    CLHEP::HepSymMatrix D = S.similarityT(U);   // D = U.T() S U = Sdiag
    for (int i = 1; i <= S.num_row(); i++) {
      double s2 = D(i,i);
      if ( s2 <= 0 ) {
	return false;
      }
    }
    return true;
  }
}

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
  sigmaGM[0][0] = pow(sigmaX,2); 
  sigmaGM[1][1] = pow(sigmaXp,2); 
  sigmaGM[2][2] = pow(sigmaY,2); 
  sigmaGM[3][3] = pow(sigmaYp,2);
  sigmaGM[4][4] = pow(sigmaT,2); 
  sigmaGM[5][6] = pow(sigmaE,2);

  // Create multi dim gaussian generator
  CreateMultiGauss();
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
  CreateMultiGauss();
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

  CreateMultiGauss();
  return;
}

void BDSBunchGaussian::CreateMultiGauss() {
  /// check if sigma matrix is positive definite
  /// if not add small offset and cout warning
  
  if (!isPositiveDefinite(sigmaGM)) {
    G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is not positive definite" << G4endl;
    G4cout << sigmaGM << G4endl;
    G4cout << __METHOD_NAME__ << "adding a small error to zero diagonal elements" << G4endl;

    double small_error = 1e-12;
    
    for (int i=0; i<6; i++) {
      if (sigmaGM[i][i]==0) {
	sigmaGM[i][i] += small_error;
      }
    }
    
    if (!isPositiveDefinite(sigmaGM)) {
      G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is still not positive definite" << G4endl;
      G4cout << sigmaGM << G4endl;
      G4cout << __METHOD_NAME__ << "adding a small error to all elements" << G4endl;
      for (int i=0; i<6; i++) {
	for (int j=0; j<6; j++) {
	  if (sigmaGM[i][j]==0) {
	    sigmaGM[i][j] += small_error;
	  }
	}
      }
      if (!isPositiveDefinite(sigmaGM)) {
	G4cout << __METHOD_NAME__ << "ERROR bunch generator sigma matrix is still not positive definite, giving up" << G4endl;
	G4cout << sigmaGM << G4endl;
	exit(1);
      }
    }
  }
 
  if(GaussMultiGen != NULL) delete GaussMultiGen;
  GaussMultiGen = new CLHEP::RandMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM); 
  return;
}

void BDSBunchGaussian::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				       G4double& xp, G4double& yp, G4double& zp,
				       G4double& t , G4double&  E, G4double& weight) {
  CLHEP::HepVector v = GaussMultiGen->fire();
  x0 = v[0];
  xp = v[1];
  y0 = v[2];
  yp = v[3];
  t  = v[4];
  zp = 0.0;
  z0 = Z0*CLHEP::m + t*CLHEP::c_light;
  E  = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * v[5];
  
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp =  sqrt(1.-xp*xp -yp*yp);

  weight = 1.0;
  return;
}
