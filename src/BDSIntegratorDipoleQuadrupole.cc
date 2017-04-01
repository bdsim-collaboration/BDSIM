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
  dipole(new BDSIntegratorDipole2(eqOfMIn, minimumRadiusOfCurvatureIn)),
  angle((*strength)["angle"]),
  arcLength((*strength)["length"])
{
  radiusOfCurvature = arcLength / angle;
  chordLength       = 2 * radiusOfCurvature * sin(angle*0.5);
  radiusAtChord     = radiusOfCurvature * cos(angle*0.5);
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

  G4double quadDelta[7];
  //  for (G4int i = 0; i < 3; i++)
  //  {
  //   quadDelta[i] = 

  G4double dipoleOut[7];
  G4double dipoleErr[7];
  dipole->Stepper(yIn, dydx, h, dipoleOut, dipoleErr);
}

BDSStep BDSIntegratorDipoleQuadrupole::GlobalToCurvilinear(G4ThreeVector position,
							   G4ThreeVector unitMomentum,
							   G4double      h,
							   G4bool        useCurvilinearWorld)
{
  BDSStep local = ConvertToLocal(position, unitMomentum, h, useCurvilinearWorld);

  // we could test on finite angle here, but we assume this integrator is only
  // used where necessary as opposed to just a dipole or quadrupole.
  
  G4ThreeVector localPos   = local.PreStepPoint();
  G4ThreeVector localMom   = local.PostStepPoint();
  G4double      localZ     = localPos.z();
  
  G4ThreeVector unitY      = G4ThreeVector(0,1,0); // nominal dipole field direction
  G4ThreeVector localUnitY = ConvertAxisToLocal(unitY, useCurvilinearWorld);

  // This will range from -angle/2 to +angle/2
  G4double partialAngle = atan(localZ / radiusAtChord);

  G4ThreeVector localMomCL = localMom.rotate(partialAngle, localUnitY);

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
