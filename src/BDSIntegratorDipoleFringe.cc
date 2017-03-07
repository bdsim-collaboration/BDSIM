#include "BDSDebug.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

BDSIntegratorDipoleFringe::BDSIntegratorDipoleFringe(BDSMagnetStrength const* strength,
						     G4Mag_EqRhs*             eqOfMIn,
						     G4double                 minimumRadiusOfCurvature):
  BDSIntegratorDipole2(eqOfMIn, minimumRadiusOfCurvature),
  polefaceAngle((*strength)["polefaceangle"]),
  fringeCorr((*strength)["fringecorr"])
{;}

void BDSIntegratorDipoleFringe::Stepper(const G4double yIn[],
					const G4double dydx[],
					const G4double h,
					G4double       yOut[],
					G4double       yErr[])
{
  // do the dipole kick using base class
  BDSIntegratorDipole2::Stepper(yIn, dydx, h, yOut, yErr);

  // don't do fringe kick if we're sampling the field  for a long step
  if (h > 1*CLHEP::cm)
    {return;}

  G4ThreeVector pos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  
  // global to local (curvilinear) - 'true' = use local volume for transform
  BDSStep    localPosMom = ConvertToLocal(pos, mom, 0.5*CLHEP::um, true);
  G4ThreeVector localPos = localPosMom.PreStepPoint();
  G4ThreeVector localMom = localPosMom.PostStepPoint();

  // check for forward going paraxial particles - only 
  if (localMom.unit().z() < 0.9)
    {return;}
  
  // get radius of curvature already calculated in base class step
  G4double rho = GetRadHelix();

  // calculate fringe field kick
  G4double         y0 = localPos[1];
  G4double       kick = y0 * tan(polefaceAngle - fringeCorr) / rho;
  G4ThreeVector yKick = G4ThreeVector(0,1,0); // unit y in curvilinear coords
  G4ThreeVector yKickGlobal = ConvertAxisToGlobal(yKick, true);
  yKickGlobal *= -kick;

  // update output momentum coordinates - i+3 for momentum
  for (G4int i = 0; i < 3; i++)
    {yOut[i+3] = yOut[i+3] + yKickGlobal[i];}
}
