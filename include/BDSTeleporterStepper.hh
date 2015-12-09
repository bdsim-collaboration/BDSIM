#ifndef BDSTELEPORTERSTEPPER_HH
#define BDSTELEPORTERSTEPPER_HH

#include "BDSAuxiliaryNavigator.hh"

#include "globals.hh" //G4 types and exceptions
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include <vector>

class BDSTeleporterStepper:
  public G4MagIntegratorStepper, public BDSAuxiliaryNavigator
{
public:
  BDSTeleporterStepper(G4Mag_EqRhs* EqRhs);
  ~BDSTeleporterStepper();
  
  void Stepper(const G4double y[],
	       const G4double dydx[],
	       const G4double h,
	       G4double yout[],
	       G4double yerr[]);
  
  G4int    IntegratorOrder() const {return 2;}
  G4double DistChord()       const {return 0;}
  void     StepperName();

private:
  G4bool verboseStep;
  G4int  verboseEventNumber;
  G4int  nvar;
  //  std::vector<G4int> turnnumberrecord;
  //  G4int  turnstaken;
  G4ThreeVector teleporterdelta;
};

inline void BDSTeleporterStepper::StepperName()
{G4cout << "BDSTeleporterStepper" << G4endl;}

#endif
