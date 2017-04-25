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

  G4complex kick(0,0);
  
  //apply kick
  xp1 -= kick.imag();
  yp1 += kick.real();
  zp1 = std::sqrt(1 - std::pow(xp1,2) - std::pow(yp1,2));
  if (std::isnan(zp1))
    {zp1 = zp;}

  // xp1 or yp1 may be > 1, so isnan check also needed for zp1.
  if (std::isnan(zp1) || (zp1 < 0.9))
    {
      AdvanceDriftMag(yIn, h, yOut);
      SetDistChord(0);
      return;
    }
  
  G4ThreeVector localPosOut     = G4ThreeVector(x1, y1, z1);
  G4ThreeVector localMomUnitOut = G4ThreeVector(xp1, yp1, zp1);

  ConvertToGlobal(localPosOut,localMomUnitOut,momMag,yOut);

  for (G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
}
