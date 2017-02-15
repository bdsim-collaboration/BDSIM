#ifndef BDSINTEGRATORDIPOLE2_H
#define BDSINTEGRATORDIPOLE2_H

#include "globals.hh"
#include "G4MagHelicalStepper.hh"
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;

class BDSIntegratorDipole2: public G4MagHelicalStepper
{
public:
  BDSIntegratorDipole2(G4Mag_EqRhs* eqOfMIn,
		       G4double     minimumRadiusOfCurvature);

  virtual ~BDSIntegratorDipole2(){;}

  virtual void DumbStepper(const G4double yIn[],
			   G4ThreeVector  field,
			   G4double       stepLength,
			   G4double       yOut[]);

  virtual void Stepper( const G4double yInput[],
			const G4double*,
			G4double hstep,
			G4double yOut[],
			G4double yErr[]  );

  virtual G4int IntegratorOrder() const {return 2;}

  void AdvanceHelixForSpiralling(const G4double yIn[],
				 G4ThreeVector  field,
				 G4double       stepLength,
				 G4double       yOut[],
				 G4double       yErr[]);

private:

  G4double minimumRadiusOfCurvature;
  
};

#endif
