#include "BDSBunchSquare.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchSquare::BDSBunchSquare() :
  BDSBunch(), envelopeX(0.0), envelopeY(0.0),
  envelopeXp(0.0), envelopeYp(0.0), envelopeT(0.0), envelopeE(0.0)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchSquare::~BDSBunchSquare()
{
  delete FlatGen;
}

void BDSBunchSquare::SetOptions(const GMAD::Beam& beam,
				G4Transform3D beamlineTransformIn)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunch::SetOptions(beam, beamlineTransformIn);
  SetEnvelopeX(beam.envelopeX); 
  SetEnvelopeY(beam.envelopeY);
  SetEnvelopeXp(beam.envelopeXp);
  SetEnvelopeYp(beam.envelopeYp);
  SetEnvelopeT(beam.envelopeT);
  SetEnvelopeE(beam.envelopeE); 
}

void BDSBunchSquare::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  x0 = X0  * CLHEP::m;
  y0 = Y0  * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  z0 = Z0  * CLHEP::m + (T0 - envelopeT * (1.-2.*FlatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  
  if(envelopeX !=0) x0  += envelopeX  * (1-2*FlatGen->shoot()) * CLHEP::m;
  if(envelopeY !=0) y0  += envelopeY  * (1-2*FlatGen->shoot()) * CLHEP::m;
  if(envelopeXp !=0) xp += envelopeXp * (1-2*FlatGen->shoot()) * CLHEP::rad;
  if(envelopeYp !=0) yp += envelopeYp * (1-2*FlatGen->shoot()) * CLHEP::rad;
  
  zp = CalculateZp(xp,yp,Zp0);

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  t = 0 * CLHEP::s;
  E = E0 * CLHEP::GeV * (1 + envelopeE * (1-2*FlatGen->shoot()));

  weight = 1.0;
  return; 
}
