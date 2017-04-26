#include "BDSIntegratorDipole2.hh"

#include "globals.hh"
#include "G4ClassicalRK4.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSIntegratorDipole2::BDSIntegratorDipole2(G4Mag_EqRhs* eqOfMIn,
					   G4double     minimumRadiusOfCurvatureIn):
  G4MagHelicalStepper(eqOfMIn),
  minimumRadiusOfCurvature(minimumRadiusOfCurvatureIn),
  eqOfM(eqOfMIn)
{;}

void BDSIntegratorDipole2::DumbStepper(const G4double yIn[],
				       G4ThreeVector  field,
				       G4double       stepLength,
				       G4double       yOut[])
{
  AdvanceHelix(yIn, field, stepLength, yOut);
}

void BDSIntegratorDipole2::Stepper(const G4double yIn[],
				   const G4double[] /*dydx*/,
				   G4double       h,
				   G4double       yOut[],
				   G4double       yErr[])
{
  // Protect against very small steps.
  if (h < 1e-12)
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      return;
    }
  
  // Extra storage arrays.
  G4double yTemp[7], yTemp2[7];

  // Neutral particles drift through.
  if(eqOfM->FCof() == 0)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      // can't set distchord directly due to G4MagHelicaStepper
      // function not virtual there.
      // this combination will give a rational answer given the logic
      // in G4MagHelicalStepper::DistChord
      SetAngCurve(10); // > 2pi
      SetRadHelix(0);
      return;
    }
  
  // Arrays for field querying (g4 interface)
  G4double bO[4]; // original location field value
  eqOfM->GetFieldValue(yIn, bO);
  G4ThreeVector bOriginal = G4ThreeVector(bO[0],bO[1],bO[2]);

  // Do a full step - the result we use
  AdvanceHelix(yIn, bOriginal, h, yOut);
  // cache some calculated parameters as there's no way to cache the distchord
  // directly as the G4MagHelicalStepper class doesn't make DistChord() virtual.
  G4double ang = GetAngCurve();
  G4double rad = GetRadHelix();

  // Now we have the radius of curvature to check on.
  // If it's smaller than limit, we artificially advance the particle
  // along its helix axis (parallel to the field) so it'll hit something
  // and finish in a timely manner.
  const G4double radiusOfCurvature = GetRadHelix();
  if (std::abs(radiusOfCurvature) < minimumRadiusOfCurvature)
    {
      AdvanceHelixForSpiralling(yIn, bOriginal, h, yOut, yErr);
      return;
    }

  // error estimation
  // if it's a very small step - no error - safer as differences between large positions
  // for very small steps can have numerical precision issues.
  // no need to update ang and rad.
  if (h < 1e-9) // 1e-9 is 1pm (in g4 units) - the minimum length scale of bdsim
    {
      for (G4int i = 0; i < 6; i++)
        {yErr[i] = 0;}
      return; // saves long if else
    }
  
  // normal error estimation - do two half steps and compare difference to
  // the result from one full step
  AdvanceHelix(yIn, bOriginal, h*0.5, yTemp); // first step

  // resample field at midway point (although if pure dipole, this is
  // unnecessary) - could go outside the range of the field though
  G4double bM[4]; // mid point location field value
  GetEquationOfMotion()->GetFieldValue(yTemp, bM);
  G4ThreeVector bMid = G4ThreeVector(bM[0],bM[1],bM[2]);

  AdvanceHelix(yTemp, bMid, h*0.5, yTemp2); // second step
  
  // Error estimation
  for(G4int i = 0; i < 6; i++)
    {yErr[i] = std::abs(yOut[i] - yTemp2[i]);}

  // Update parameters that distchord will be calcualted from from full step info.
  SetAngCurve(ang);
  SetRadHelix(rad);
}

void BDSIntegratorDipole2::AdvanceHelixForSpiralling(const G4double yIn[],
						     G4ThreeVector  field,
						     G4double       h,
						     G4double       yOut[],
						     G4double       yErr[])
{
  G4ThreeVector fieldUnit = field.unit();
  G4double momMag = G4ThreeVector(yIn[3], yIn[4], yIn[5]).mag();
  
  // Artificially change momentum to be along field direction.
  G4ThreeVector    momNew = fieldUnit*momMag;

  // Create updated array for tracking.
  G4double yModified[7];
  for (G4int i = 0; i < 3; i++)
    {
      yModified[i]     = yIn[i];    // position stays the same
      yModified[i + 3] = momNew[i]; // update momentum
    }

  // Simply advance as a drift along this vector.
  AdvanceDriftMag(yModified, h, yOut);

  // set error to be low.
  for(G4int i = 0; i < 3; i++)
    {yErr[i] = 1e-20;}
  for(G4int i = 3; i < 6; i++)
    {yErr[i] = 1e-40;}
}
