#ifndef BDSINTEGRATORTELEPORTER_H
#define BDSINTEGRATORTELEPORTER_H

#include "BDSIntegratorBase.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

class BDSIntegratorTeleporter: public BDSIntegratorBase
{
public:
  BDSIntegratorTeleporter(G4Mag_EqRhs* const  eqRHSIn,
			  const G4ThreeVector teleporterDelta);

  virtual ~BDSIntegratorTeleporter(){;}
  
  void Stepper(const G4double y[],
	       const G4double dydx[],
	       const G4double h,
	       G4double yout[],
	       G4double yerr[]);

private:
  /// The offset the teleport should transport particles by
  G4ThreeVector teleporterDelta;
};

#endif
