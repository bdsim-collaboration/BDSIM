#include "BDSBunchGaussian.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchGaussian::BDSBunchGaussian(): 
  BDSBunch(),
  sigmaX(0.0),sigmaY(0.0),sigmaXp(0.0),sigmaYp(0.0),
  meansGM(CLHEP::HepVector(6)),
  sigmaGM(CLHEP::HepSymMatrix(6)),
  GaussMultiGen(nullptr)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

BDSBunchGaussian::~BDSBunchGaussian()
{
  delete GaussMultiGen;
}

void BDSBunchGaussian::SetOptions(const GMAD::Beam& beam,
				  G4Transform3D beamlineTransformIn)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunch::SetOptions(beam, beamlineTransformIn);
  
  SetSigmaX(beam.sigmaX); 
  SetSigmaY(beam.sigmaY);
  SetSigmaXp(beam.sigmaXp);
  SetSigmaYp(beam.sigmaYp);
  
  meansGM[0]    = X0;
  meansGM[1]    = Xp0;
  meansGM[2]    = Y0;
  meansGM[3]    = Yp0;
  meansGM[4]    = T0;
  meansGM[5]    = 1;

  if(strcmp(beam.distrType.data(),"gaussmatrix") == 0) {
    sigmaGM[0][0] = beam.sigma11; 
    sigmaGM[0][1] = beam.sigma12;
    sigmaGM[0][2] = beam.sigma13;
    sigmaGM[0][3] = beam.sigma14;
    sigmaGM[0][4] = beam.sigma15;
    sigmaGM[0][5] = beam.sigma16;  
    sigmaGM[1][1] = beam.sigma22;
    sigmaGM[1][2] = beam.sigma23;
    sigmaGM[1][3] = beam.sigma24;
    sigmaGM[1][4] = beam.sigma25;
    sigmaGM[1][5] = beam.sigma26;  
    sigmaGM[2][2] = beam.sigma33;
    sigmaGM[2][3] = beam.sigma34;
    sigmaGM[2][4] = beam.sigma35;
    sigmaGM[2][5] = beam.sigma36;  
    sigmaGM[3][3] = beam.sigma44;
    sigmaGM[3][4] = beam.sigma45;
    sigmaGM[3][5] = beam.sigma46;  
    sigmaGM[4][4] = beam.sigma55;
    sigmaGM[4][5] = beam.sigma56;  
    sigmaGM[5][5] = beam.sigma66;
  }
  else if (strcmp(beam.distrType.data(),"gauss") == 0) 
  {    
    sigmaGM[0][0] = std::pow(beam.sigmaX,2); 
    sigmaGM[1][1] = std::pow(beam.sigmaXp,2); 
    sigmaGM[2][2] = std::pow(beam.sigmaY,2); 
    sigmaGM[3][3] = std::pow(beam.sigmaYp,2);       
    sigmaGM[4][4] = std::pow(beam.sigmaT,2); 
    sigmaGM[5][5] = std::pow(beam.sigmaE,2);
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
  z0 = Z0 * CLHEP::m + t * CLHEP::c_light;

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  E  = E0 * CLHEP::GeV * v[5];
  zp = CalculateZp(xp,yp,Zp0);

  weight = 1.0;
  return;
}
