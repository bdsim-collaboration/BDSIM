#include "BDSIntegratorEuler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorEuler::BDSIntegratorEuler(G4Mag_EqRhs* eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{;}

void BDSIntegratorEuler::Stepper(const G4double yIn[],
				 const G4double dydx[],
				 G4double       h,
				 G4double       yOut[],
				 G4double       yErr[])
{ 
  const G4ThreeVector pos     = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  const G4ThreeVector mom     = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  // unit momentum already provided for us
  const G4ThreeVector momU    = G4ThreeVector(dydx[0], dydx[1], dydx[2]); 
  const G4ThreeVector a_start = G4ThreeVector(dydx[3], dydx[4], dydx[5]);
  const G4double      hSqrd   = std::pow(h, 2);
  const G4double      halfSL  = h*0.5;

  // calcualte the position half step length for drifting with no force
  // 'phalf' => plus half step
  G4ThreeVector pos_phalf = pos + momU*halfSL;

  // construct mixed coordinate input for field calculation
  // half drift position but original momentum vector (w.r.t. original position, also unit)
  G4double mixedPosMom[7];
  for (G4int i = 0; i < 3; i++)
    {
      mixedPosMom[i]   = pos_phalf[i];
      mixedPosMom[i+3] = momU[i];
    }
  mixedPosMom[6] = yIn[6];

  G4double potential[7]; // output array for g4 query
  RightHandSide(mixedPosMom, potential); // query field and calculate vector potential
  // get the vector potential bit from the array
  G4ThreeVector a_phalf = G4ThreeVector(potential[3], potential[4], potential[5]);

  // new coordinates
  // pos_new = pos + p.unit()*h + (A*h^2)/2*p.mag()
  G4double      factor  = hSqrd*0.5/mom.mag();  // calculate common factors first
  G4ThreeVector pos_new = pos + momU*h + a_phalf*factor;
  G4ThreeVector mom_new = mom + a_phalf*h;

  // error calculation
  G4ThreeVector a_diff  = a_phalf - a_start;
  G4ThreeVector pos_err = a_diff*factor;
  G4ThreeVector mom_err = a_diff*h;

  // write out output values and errors to arrays
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = pos_new[i];
      yOut[i+3] = mom_new[i];
      yErr[i]   = std::abs(pos_err[i]);
      yErr[i+3] = std::abs(mom_err[i]);
    }
  
  // ((average of new and old) - mid point from drift ) .mag()
  // both are straight lines, but it's an approximately close
  G4double dc = (0.5*(pos_new + pos) - pos_phalf).mag();
  SetDistChord(dc);
}
