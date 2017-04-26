#include "BDSIntegratorKickerThin.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorKickerThin::BDSIntegratorKickerThin(BDSMagnetStrength const* strength,
						 G4double                 brho,
						 G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{;}

void BDSIntegratorKickerThin::Stepper(const G4double yIn[],
					 const G4double[] /*dydx[]*/,
					 const G4double h,
					 G4double       yOut[],
					 G4double       yErr[])
{
  G4ThreeVector pos    = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom    = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag = mom.mag();
  
  // global to local
  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();

  // only use for forward paraxial momenta, else advance particle as if in a drift
  if (localMomUnit.z() < 0.9)
    {
      AdvanceDriftMag(yIn, h, yOut);
      SetDistChord(0);
      return;
    }

  // Transverse position stays the same and the particle is advanced by h along local z.
  G4double x1  = localPos.x();
  G4double y1  = localPos.y();
  G4double z1  = localPos.z();
  z1 += h;
  
  G4double xp1 = localMomUnit.x();
  G4double yp1 = localMomUnit.y();
  G4double zp1 = localMomUnit.z();


  
  G4ThreeVector localPosOut     = G4ThreeVector(x1, y1, z1);
  G4ThreeVector localMomUnitOut = G4ThreeVector(xp1, yp1, zp1);

  ConvertToGlobal(localPosOut,localMomUnitOut,momMag,yOut);

  for (G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
}
