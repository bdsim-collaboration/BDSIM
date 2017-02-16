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
					const G4double stepLength,
					G4double       yOut[],
					G4double       yErr[])
{
  // do the dipole kick using base class
  BDSIntegratorDipole2::Stepper(yIn, dydx, stepLength, yOut, yErr);

  G4ThreeVector globalPos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector globalMom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  
  // global to local - 'false' = use local volume for transform
  BDSStep    localPosMom = ConvertToLocal(globalPos, globalMom, stepLength, false);
  G4ThreeVector localPos = localPosMom.PreStepPoint();
  G4ThreeVector localMom = localPosMom.PostStepPoint();

  // check for forward going paraxial particles - only 
  if (localMom.z() < 0.9)
    {return;}
  
  // get radius of curvature already calculated in base class step
  G4double rho = GetRadHelix();

  // calculate fringe field kick
  // TBC - should this vary charge or is that sign encapsulated in rho?
  G4double   y0 = localPos[1];
  G4double kick = y0 * tan(polefaceAngle - fringeCorr) / rho;
  G4double   yp = localMom[1];
  G4double  yp1 = yp - kick;

  // update vector with new y kick
  localMom.setY(yp1);

  // scale appropriately
  localMom*globalMom.unit();

  // convert back to global coordinates - 'false' = use local volume transform.
  G4ThreeVector globalPosOut = ConvertToGlobal(localPos, false);
  G4ThreeVector globalMomOut = ConvertAxisToGlobal(globalPos, localMom, false);
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = globalPosOut[i];
      yOut[i+3] = globalMomOut[i];
    }
}
