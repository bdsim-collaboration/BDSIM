#ifndef BDSINTEGRATORTELEPORTER_H
#define BDSINTEGRATORTELEPORTER_H

#include "BDSIntegratorMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;

/**
 * @brief Custom unphysical integrator to advance particle in teleporter.
 *
 * @author Laurie Nevay
 */

class BDSIntegratorTeleporter: public BDSIntegratorMag
{
public:
  BDSIntegratorTeleporter(G4Mag_EqRhs*  eqOfMIn,
			  G4ThreeVector teleporterDelta);

  virtual ~BDSIntegratorTeleporter(){;}
  
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

private:
  /// The offset the teleport should transport particles by
  G4ThreeVector teleporterDelta;
};

#endif
