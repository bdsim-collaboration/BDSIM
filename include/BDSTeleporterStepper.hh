#ifndef BDSTeleporterStepper_h
#define BDSTeleporterStepper_h 1

#include "globals.hh" //G4 types and exceptions
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include <vector>

class BDSTeleporterStepper : public G4MagIntegratorStepper
{

public:  // with description
  BDSTeleporterStepper(G4Mag_EqRhs *EqRhs);
  ~BDSTeleporterStepper();
  
  void Stepper( const G4double y[],
		const G4double dydx[],
		const G4double h,
		G4double yout[],
		G4double yerr[]  );
  G4int    IntegratorOrder() const {return 2;}
  G4double DistChord()       const {return 0;}
  void     StepperName();

protected:
  void AdvanceHelix(const G4double yIn[],
		    G4ThreeVector Bfld,
		    G4double h,
		    G4double yDrift[]);

private:
  G4bool verboseStep;
  G4int  verboseEventNumber;
  G4int  nvar;
  //  std::vector<G4int> turnnumberrecord;
  //  G4int  turnstaken;
  G4ThreeVector teleporterdelta;
};

inline void BDSTeleporterStepper::StepperName()
{G4cout<<"BDSTeleporterStepper"<<G4endl;}

#endif
