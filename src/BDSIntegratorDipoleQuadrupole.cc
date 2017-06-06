#include "BDSDebug.hh"
#include "BDSIntegratorDipole2.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIntegratorDipoleQuadrupole::BDSIntegratorDipoleQuadrupole(BDSMagnetStrength const* strength,
							     G4double                 brho,
							     G4Mag_EqRhs*             eqOfMIn,
							     G4double minimumRadiusOfCurvatureIn):
  BDSIntegratorQuadrupole(strength, brho, eqOfMIn, minimumRadiusOfCurvatureIn),
  dipole(new BDSIntegratorDipole2(strength, eqOfMIn, minimumRadiusOfCurvatureIn)),
  angle((*strength)["angle"]),
  arcLength((*strength)["length"])
{
  radiusOfCurvature = arcLength / angle;
  chordLength       = 2 * radiusOfCurvature * sin(angle*0.5);
  radiusAtChord     = radiusOfCurvature * cos(angle*0.5);
  unitField         = G4ThreeVector(0,(*strength)["field"],0).unit();
}

BDSIntegratorDipoleQuadrupole::~BDSIntegratorDipoleQuadrupole()
{
  delete dipole;
}

void BDSIntegratorDipoleQuadrupole::Stepper(const G4double yIn[],
					    const G4double dydx[],
					    const G4double h,
					    G4double       yOut[],
					    G4double       yErr[])
{
  G4double quadOut[7];
  G4double quadErr[7];

  BDSIntegratorQuadrupole::Stepper(yIn, dydx, h, quadOut, quadErr);
  
  dipole->Stepper(yIn, dydx, h, yOut, yErr);

  // If the particle might spiral, we return and just use the dipole only component
  // Aimed at particles of much lower momentum than the design energy.
  G4double dipoleDC          = dipole->DistChord();
  G4double radiusOfCurvature = dipole->RadiusOfHelix();
  if (dipoleDC > 0.3*radiusOfCurvature)
    {return;}

  // convert to true curvilinear
  G4ThreeVector globalPos  = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector globalMom  = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  BDSStep       localCL    = GlobalToCurvilinear(globalPos, globalMom, h, false);
  G4ThreeVector localCLPos = localCL.PreStepPoint();
  G4ThreeVector localCLMom = localCL.PostStepPoint();
  
  // calculate new position - TBC
  G4ThreeVector localCLPosOut = localCLPos;
  G4ThreeVector localCLMomOut = localCLMom;

  // convert to global coordinates for output
  BDSStep globalOut = CurvilinearToGlobal(localCLPosOut, localCLMomOut, false);
  G4ThreeVector globalPosOut = globalOut.PreStepPoint();
  G4ThreeVector globalMomOut = globalOut.PostStepPoint();

  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]     = globalPosOut[i];
      yOut[i + 3] = globalMomOut[i];
      yErr[i]     = 0;
    }
}

void BDSIntegratorDipoleQuadrupole::OneStep(G4ThreeVector  posIn,
					    G4ThreeVector  momIn,
					    G4double       h,
					    G4ThreeVector& posOut,
					    G4ThreeVector& momOut) const
{
  G4double delta     = 0;
  
  G4double theta     = 0;
  G4double kappa     = 0;
  G4double sqrtKappa = std::sqrt(kappa);
  if (std::isnan(sqrtKappa))
    {sqrtKappa = 0;}

  G4double x0  = posIn.x();
  G4double y0  = posIn.y();
  G4double s0  = posIn.z();
  G4double xp0 = momIn.x();
  G4double yp0 = momIn.y();

  G4double cosTheta  = cos(theta);
  G4double sinTheta  = sin(theta);
  G4double coshTheta = cosh(theta);
  G4double sinhTheta = sinh(theta);
  G4double m00 = cosTheta;
  G4double m01 = - sqrtKappa * sinTheta;
  G4double m10 = sinTheta / sqrtKappa;
  G4double m11 = cosTheta;
  G4double m05 = (1 - cosTheta) / sqrtKappa;
  G4double m15 = sinTheta;
  G4double m22 = coshTheta;
  G4double m23 = sinhTheta / sqrtKappa;
  G4double m32 = sqrtKappa * sinhTheta;
  G4double m33 = coshTheta;

  G4double x1  = m00*x0 + m10*xp0 + m05*delta;
  G4double xp1 = m01*x0 + m11*xp0 + m15*delta;
  G4double y1  = m22*y0 + m23*yp0;
  G4double yp1 = m32*y0 + m33*yp0;
  G4double s1  = s0 + h;

  posOut[0] = x1;
  posOut[1] = y1;
  posOut[2] = s1;
  momOut[0] = xp1;
  momOut[1] = yp1;
  momOut[2] = momIn[2];
}

BDSStep BDSIntegratorDipoleQuadrupole::GlobalToCurvilinear(G4ThreeVector position,
							   G4ThreeVector unitMomentum,
							   G4double      h,
							   G4bool        useCurvilinearWorld)
{
  BDSStep local = ConvertToLocal(position, unitMomentum, h, useCurvilinearWorld);

  // We could test on finite angle here, but we assume this integrator is only
  // used where necessary as opposed to just a dipole or quadrupole.
  // If the angle is 0, there is no need for a further transform.
  
  G4ThreeVector localPos   = local.PreStepPoint();
  G4ThreeVector localMom   = local.PostStepPoint();
  G4double      localZ     = localPos.z();
  G4ThreeVector localUnitF = ConvertAxisToLocal(unitField, useCurvilinearWorld);

  // This will range from -angle/2 to +angle/2
  G4double partialAngle = atan(localZ / radiusAtChord);

  G4ThreeVector localMomCL = localMom.rotate(partialAngle, localUnitF);

  G4ThreeVector unitX      = G4ThreeVector(1,0,0);
  G4ThreeVector localUnitX = ConvertAxisToLocal(unitX, useCurvilinearWorld);
  G4double      dx         = radiusOfCurvature * (1 - cos(partialAngle));
  G4ThreeVector dpos       = localUnitX * dx;
  G4ThreeVector localPosCL = localPos + dpos;
  
  return BDSStep(localPosCL, localMomCL);
}


BDSStep BDSIntegratorDipoleQuadrupole::CurvilinearToGlobal(G4ThreeVector localPositionCL,
							   G4ThreeVector localMomentumCL,
							   G4bool        useCurvilinearWorld)
{
  return ConvertToGlobalStep(localPositionCL, localMomentumCL, useCurvilinearWorld);
}
