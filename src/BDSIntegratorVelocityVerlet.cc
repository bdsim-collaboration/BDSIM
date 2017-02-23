#include "BDSIntegratorVelocityVerlet.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorVelocityVerlet::BDSIntegratorVelocityVerlet(G4Mag_EqRhs* eqOfMIn):
  G4MagIntegratorStepper(eqOfMIn, 6),
  distChord(0)
{
  backupIntegrator = new G4ClassicalRK4(eqOfMIn, 6);
}

BDSIntegratorVelocityVerlet::~BDSIntegratorVelocityVerlet()
{
  delete backupIntegrator;
}

void BDSIntegratorVelocityVerlet::Stepper(const G4double yIn[],
					  const G4double dydx[],
					  G4double       stepLength,
					  G4double       yOut[],
					  G4double       yErr[])
{
  G4double yTemp[7], yTwoHalf[7], yErr2[7];
  
  // Do two half steps
  SimpleStepper(yIn,   dydx, stepLength*0.5, yTemp, yErr2);
  SimpleStepper(yTemp, dydx, stepLength*0.5, yTwoHalf, yErr2); 
  
  // Do a full Step
  SimpleStepper(yIn, dydx, stepLength, yOut, yErr2);

  for(G4int i = 0; i < 6; i++)
    {
      yErr[i] = yOut[i] - yTwoHalf[i];
    }
}

void BDSIntegratorVelocityVerlet::SimpleStepper(const G4double yIn[],
						const G4double dydx[],
						G4double       stepLength,
						G4double       yOut[],
						G4double       yErr[])
{
  const G4ThreeVector globalPos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  const G4ThreeVector globalMom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  const G4double      stepLengthSqrd = pow(stepLength, 2);

  const G4ThreeVector a = G4ThreeVector(dydx[3], dydx[4], dydx[5]);

  G4ThreeVector term1 = globalMom.unit()*stepLength;
  G4ThreeVector term2 = 0.5*a*stepLengthSqrd;
  G4ThreeVector globalPosNew = globalPos + term1 + term2;

  G4double globalPosNewArr[4];
  for (G4int i = 0; i < 3; i++)
    {globalPosNewArr[i] = globalPosNew[i];}
  globalPosNewArr[3] = yIn[7];

  G4double dydxPosNew[7];
  ComputeRightHandSide(globalPosNewArr, dydxPosNew);

  G4ThreeVector aNew = G4ThreeVector(dydxPosNew[3], dydxPosNew[4], dydxPosNew[5]);

  G4ThreeVector globalMomNew = globalMom + 0.5 * (a + aNew) * stepLength;

  //globalMom.unit() + h*anew

  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = globalPosNew[i];
      yOut[i+3] = globalMomNew[i];
      yErr[i]   = 1e-15;
      yErr[i+3] = 1e-20;
    }

  distChord = stepLengthSqrd * a.mag() / 8;
  
  /*
  
  // calculate approximate mid point
  const G4ThreeVector globalPosHalfStep = globalPos + globalMom*0.5*stepLength;

  // use field from there
  // needs to be in array form for Geant4
  G4double globalPosHalfStepArr[4];
  for (G4int i = 0; i < 3; i++)
    {globalPosHalfStepArr[i] = globalPosHalfStep[i];}
  globalPosHalfStepArr[3] = yIn[7]; // copy time over
  G4double dydxHalfStep[7];

  // This evaluates the field at the new position and calculates the new
  // vector potential.
  ComputeRightHandSide(globalPosHalfStepArr, dydxHalfStep);
  // dydx[0-2] are normalised momenta
  // dydx[3-5] are (1/|p|) * (p cross B)
  const G4ThreeVector globalF = G4ThreeVector(dydxHalfStep[3], dydxHalfStep[4], dydxHalfStep[5]);
  
  G4ThreeVector globalPosDelta = globalMom * stepLength + globalF * stepLengthSqrd * 0.5;
  G4ThreeVector globalPosNew   = globalPos + globalPosDelta;
  G4ThreeVector globalMomDelta = stepLength*globalF;
  G4ThreeVector globalMomNew   = globalMom + globalMomDelta;

  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = globalPosNew[i];
      yOut[i+3] = globalMomNew[i];
      yErr[i]   = 1e-15;
      yErr[i+3] = 1e-20;
    }

  distChord = stepLengthSqrd * globalF.mag() / 8;
  */
}
