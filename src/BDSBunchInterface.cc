#include "BDSBunchInterface.hh"

#include "BDSDebug.hh" 

#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

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

BDSBunchInterface::BDSBunchInterface(): 
  X0(0.0), Y0(0.0), Z0(0.0), T0(0.0), 
  Xp0(0.0), Yp0(0.0), Zp0(0.0), sigmaT(0.0), sigmaE(0.0)
{}

BDSBunchInterface::~BDSBunchInterface()
{;}

void BDSBunchInterface::SetOptions(GMAD::Options& opt)
{
  X0 = opt.X0;
  Y0 = opt.Y0;
  Z0 = opt.Z0;
  T0 = opt.T0;
  Xp0 = opt.Xp0;
  Yp0 = opt.Yp0;
  sigmaE = opt.sigmaE;
  sigmaT = opt.sigmaT;

  Zp0 = CalculateZp(Xp0,Yp0,opt.Zp0);
}

void BDSBunchInterface::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
					G4double& xp, G4double& yp, G4double& zp,
					G4double& t , G4double&  E, G4double& weight)
{
  x0 = (X0 + 0.0) * CLHEP::m;
  y0 = (Y0 + 0.0) * CLHEP::m;
  z0 = (Z0 + 0.0) * CLHEP::m;
  xp = (Xp0 + 0.0)* CLHEP::rad;
  yp = (Yp0 + 0.0)* CLHEP::rad;
  zp = CalculateZp(xp,yp,Zp0);
  t  = 0.0; 
  E = BDSGlobalConstants::Instance()->GetParticleKineticEnergy();
  weight = 1.0;
  return;
}

CLHEP::RandMultiGauss* BDSBunchInterface::CreateMultiGauss(CLHEP::HepRandomEngine & anEngine,
							   const CLHEP::HepVector & mu,
							   CLHEP::HepSymMatrix & sigma)
{
  /// check if sigma matrix is positive definite
  /// if not add small offset and cout warning
  
  if (!isPositiveDefinite(sigma)) {
    G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is not positive definite" << G4endl;
    G4cout << sigma << G4endl;
    G4cout << __METHOD_NAME__ << "adding a small error to zero diagonal elements" << G4endl;

    // very small number especially for time, since the sigma goes with the square
    double small_error = 1e-50;
    
    for (int i=0; i<6; i++) {
      if (sigma[i][i]==0) {
	sigma[i][i] += small_error;
      }
    }
    
    if (!isPositiveDefinite(sigma)) {
      G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is still not positive definite" << G4endl;
      G4cout << sigma << G4endl;
      G4cout << __METHOD_NAME__ << "adding a small error to all elements" << G4endl;
      for (int i=0; i<6; i++) {
	for (int j=0; j<6; j++) {
	  if (sigma[i][j]==0) {
	    sigma[i][j] += small_error;
	  }
	}
      }
      if (!isPositiveDefinite(sigma)) {
	G4cout << __METHOD_NAME__ << "ERROR bunch generator sigma matrix is still not positive definite, giving up" << G4endl;
	G4cout << sigma << G4endl;
	exit(1);
      }
    }
  }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "mean "  << G4endl
	 << mu    << G4endl
	 << __METHOD_NAME__ << "sigma " << G4endl
	 << sigma << G4endl;
#endif
  
  return new CLHEP::RandMultiGauss(anEngine,mu,sigma); 
}

G4double BDSBunchInterface::CalculateZp(G4double xp, G4double yp, G4double Zp0)const
{
  double zp;
  if (xp*xp+yp*yp > 1) {
    G4cout << __METHOD_NAME__ << "ERROR xp, yp too large, xp: " << xp << " yp: " << yp << G4endl;
    exit(1);
  }
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp = sqrt(1.-xp*xp -yp*yp);

  return zp;
}
