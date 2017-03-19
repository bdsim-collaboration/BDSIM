#include "BDSDebug.hh"
#include "BDSIntegratorDipole2.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

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
  dipole(new BDSIntegratorDipole2(eqOfMIn, minimumRadiusOfCurvatureIn))
{;}

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

  G4double dipoleOut[7];
  G4double dipoleErr[7];
  dipole->Stepper(yIn, dydx, h, dipoleOut, dipoleErr);
}

BDSStep BDSIntegratorQuadrupole::GlobalToCurvilinear(G4ThreeVector position,
						     G4ThreeVector unitMomentum,
						     G4double      h,
						     G4bool        useCurvilinearWorld)
{
  return ConvertToLocal(position, unitMomentum, h, useCurvilinearWorld);
}


BDSStep BDSIntegratorQuadrupole::CurvilinearToGlobal(G4ThreeVector localPosition,
						     G4ThreeVector localMomentum,
						     G4bool        useCurvilinearWorld)
{
  return ConvertToGlobalStep(localPosition, localMomentum, useCurvilinearWorld);
}
