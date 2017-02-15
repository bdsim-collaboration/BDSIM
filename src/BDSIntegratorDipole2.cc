#include "BDSIntegratorDipole2.hh"

#include "globals.hh"

BDSIntegratorDipole2::BDSIntegratorDipole2(G4Mag_EqRhs* eqOfMIn,
					   G4double     minimumRadiusOfCurvatureIn):
  G4MagHelicalStepper(eqOfMIn),
  minimumRadiusOfCurvature(minimumRadiusOfCurvatureIn)
{;}

void BDSIntegratorDipole2::DumbStepper(const G4double yIn[],
				       G4ThreeVector  field,
				       G4double       stepLength,
				       G4double       yOut[])
{
  AdvanceHelix(yIn, field, stepLength, yOut);
}

void BDSIntegratorDipole2::Stepper(const G4double   yIn[],
				   const G4double[] /*dydx*/,
				   G4double         stepLength,
				   G4double         yOut[],
				   G4double         yErr[])
{
  // Extra storage array.
  G4double yTemp[7];
  
  // Arrays for field querying (g4 interface)
  G4double bO[4], bM[4]; // original and mid point
  GetEquationOfMotion()->GetFieldValue(yIn, bO);
  G4ThreeVector bOriginal = G4ThreeVector(bO[0],bO[1],bO[2]);

  DumbStepper(yIn, bOriginal, stepLength, yTemp);
  G4double radiusOfCurvature = GetRadHelix();
  if (radiusOfCurvature < minimumRadiusOfCurvature)
    {
      AdvanceHelixForSpiralling(yIn, bOriginal, stepLength, yOut, yErr);
      return;
    }

  // Do two half steps
  AdvanceHelix(yIn, bOriginal, stepLength*0.5, yTemp);

  GetEquationOfMotion()->GetFieldValue(yTemp, bM);
  G4ThreeVector bMid = G4ThreeVector(bM[0],bM[1],bM[2]);

  AdvanceHelix(yTemp, bMid, stepLength*0.5, yOut);

  // Do a full Step
  AdvanceHelix(yIn, bOriginal, stepLength, yTemp);
  
  // Error estimation
  for(G4int i = 0; i < 6; i++)
    {yErr[i] = yOut[i] - yTemp[i];}
   
  return;
}

void BDSIntegratorDipole2::AdvanceHelixForSpiralling(const G4double yIn[],
						     G4ThreeVector  field,
						     G4double       stepLength,
						     G4double       yOut[],
						     G4double       yErr[])
{
  AdvanceHelix(yIn, field, stepLength, yOut, yErr);
}
