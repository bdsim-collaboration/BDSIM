#include "BDSBunchInterface.hh"

BDSBunchInterface::BDSBunchInterface() : 
  X0(0.0), Y0(0.0), Z0(0.0), T0(0.0), 
  Xp0(0.0), Yp0(0.0), Zp0(0.0), sigmaT(0.0), sigmaE(0.0)
{}

BDSBunchInterface::BDSBunchInterface(G4double sigmaTIn, G4double sigmaEIn) :
  X0(0.0), Y0(0.0), Z0(0.0), T0(0.0), Xp0(0.0), Yp0(0.0), Zp0(0.0), sigmaT(sigmaTIn), sigmaE(sigmaEIn) {

}

BDSBunchInterface::BDSBunchInterface(G4double X0In, G4double Y0In, G4double Z0In, G4double T0In,
				     G4double Xp0In, G4double Yp0In, G4double Zp0In, 
				     G4double sigmaTIn, G4double sigmaEIn) : 
  X0(X0In), Y0(Y0In), Z0(Z0In), T0(T0In), 
  Xp0(Xp0In), Yp0(Yp0In), Zp0(Zp0In), sigmaT(sigmaTIn), sigmaE(sigmaEIn)
{}

BDSBunchInterface::~BDSBunchInterface() {
}

void BDSBunchInterface::SetOptions(struct Options& opt) {
  X0 = opt.X0;
  Y0 = opt.Y0;
  Z0 = opt.Z0;
  T0 = opt.T0;
  Xp0 = opt.Xp0;
  Yp0 = opt.Yp0;
  sigmaE = opt.sigmaE;
  sigmaT = opt.sigmaT;

  if (opt.Zp0 < 0)
    Zp0 = -sqrt(1.-Xp0*Xp0-Yp0*Yp0);
  else
    Zp0 = sqrt(1.-Xp0*Xp0-Yp0*Yp0);
}

void BDSBunchInterface::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
					G4double& xp, G4double& yp, G4double& zp,
					G4double& t , G4double&  E, G4double& weight) {
  x0 = (X0 + 0.0) * CLHEP::m;
  y0 = (Y0 + 0.0) * CLHEP::m;
  z0 = (Z0 + 0.0) * CLHEP::m;
  xp = (Xp0 + 0.0)* CLHEP::rad;
  yp = (Yp0 + 0.0)* CLHEP::rad;
  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp = sqrt(1.-xp*xp -yp*yp);      
  t  = 0.0; 
  E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy();
  weight = 1.0;
  return;
}
