#include "BDSDebug.hh"
#include "BDSIntegratorFringefield.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <utility>

BDSIntegratorFringefield::BDSIntegratorFringefield(BDSMagnetStrength const* strength,
						 G4double                 brho,
						 G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorDipole(strength, brho, eqOfMIn),
  yInitial(0), yMidPoint(0), yFinal(0)
{
  polefaceAngle = (*strength)["polefaceangle"];
  angle = (*strength)["angle"];
  bField = (*strength)["field"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

void BDSIntegratorFringefield::Stepper(const G4double yInput[],
				      const G4double dydx[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  const G4double *pIn   = yInput+3;
  G4ThreeVector GlobalP = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4ThreeVector GlobalR = G4ThreeVector(yInput[0], yInput[1], yInput[2]);
  InitialiseTransform(GlobalP, GlobalR, h);
  G4ThreeVector LocalR  = ConvertToLocal(GlobalR);
  G4ThreeVector LocalRp = G4ThreeVector(dydx[0], dydx[1], dydx[2]);
  
  G4double      InitMag = GlobalP.mag();
  G4ThreeVector InitMomDir = GlobalP.unit();
 
  G4double      charge  = (eqOfM->FCof())/CLHEP::c_light;
  G4double      rho = InitMag/CLHEP::GeV/(0.299792458 * bField * charge) * CLHEP::m;
    
  if(bField==0 || eqOfM->FCof()==0)
    {
      G4ThreeVector positionMove = h * InitMomDir;
      
      yOut[0] = yInput[0] + positionMove.x();
      yOut[1] = yInput[1] + positionMove.y();
      yOut[2] = yInput[2] + positionMove.z();
      
      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();
      
      distChord = 0;
      return;
    }
    
  std::pair<G4ThreeVector,G4ThreeVector> RandRp = updatePandR(rho,h,LocalR,LocalRp);

  LocalR = RandRp.first;
  LocalRp = RandRp.second;
    
  G4double x0  = LocalR.x();
  G4double y0  = LocalR.y();
  G4double z0  = LocalR.z();
  G4double xp  = LocalRp.x();
  G4double yp  = LocalRp.y();
  G4double zp  = LocalRp.z();
    
  // initialise output varibles with input position as default
  G4double x1  = x0;
  G4double y1  = y0;
  G4double z1  = z0 + h; // new z position will be along z by step length h
  G4double xp1 = xp;
  G4double kick =(y0 * tan(polefaceAngle)/rho);
  G4double yp1 = yp - kick;
  G4double zp1 = zp;
    
  LocalR.setX(x1);
  LocalR.setY(y1);
  LocalR.setZ(z1);
  
  LocalRp.setX(xp1);
  LocalRp.setY(yp1);
  LocalRp.setZ(zp1);

  GlobalR = ConvertToGlobal(LocalR);
  GlobalP = ConvertAxisToGlobal(LocalRp);
  
  yOut[0] = GlobalR.x();
  yOut[1] = GlobalR.y();
  yOut[2] = GlobalR.z();

  yOut[3] = GlobalP.x();
  yOut[4] = GlobalP.y();
  yOut[5] = GlobalP.z();

  for(G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
}
