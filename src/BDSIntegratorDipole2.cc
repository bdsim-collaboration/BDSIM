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
  // Extra storage arrays.
  G4double yTemp[7], yTemp2[7];
  
  // Arrays for field querying (g4 interface)
  G4double bO[4], bM[4]; // original and mid point
  GetEquationOfMotion()->GetFieldValue(yIn, bO);
  G4ThreeVector bOriginal = G4ThreeVector(bO[0],bO[1],bO[2]);

  // Do a full step - the result we use
  AdvanceHelix(yIn, bOriginal, stepLength, yOut);

  // Now we have the radius of curvature to check on.
  // If it's smaller than limit, we artificially advance the particle
  // along its helix axis (parallel to the field) so it'll hit something
  // and finish in a timely manner.
  const G4double radiusOfCurvature = GetRadHelix();
  if (radiusOfCurvature < minimumRadiusOfCurvature)
    {AdvanceHelixForSpiralling(yIn, bOriginal, stepLength, yOut);}

  // Do two half steps - for error estimation
  AdvanceHelix(yIn, bOriginal, stepLength*0.5, yTemp); // first step

  GetEquationOfMotion()->GetFieldValue(yTemp, bM);
  G4ThreeVector bMid = G4ThreeVector(bM[0],bM[1],bM[2]);

  AdvanceHelix(yTemp, bMid, stepLength*0.5, yTemp2); // second step
  
  // Error estimation
  for(G4int i = 0; i < 6; i++)
    {yErr[i] = yOut[i] - yTemp2[i];}
   
  return;
}

void BDSIntegratorDipole2::AdvanceHelixForSpiralling(const G4double yIn[],
						     G4ThreeVector  field,
						     G4double       stepLength,
						     G4double       yOut[])
{
  AdvanceHelix(yIn, field, stepLength, yOut);

  G4ThreeVector unitField       = field.unit();
  G4ThreeVector correctPosition = G4ThreeVector(yOut[0], yOut[1], yOut[2]);
  G4ThreeVector newPosition     = correctPosition + 1*CLHEP::mm * unitField;
  yOut[0] = newPosition[0];
  yOut[1] = newPosition[1];
  yOut[2] = newPosition[2];
}
