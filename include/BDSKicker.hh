#ifndef BDSKICKER_H
#define BDSKICKER_H 

#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSKicker: public BDSMagnet
{
public:
  BDSKicker(G4String            name,
	    G4double            length,
	    G4double            bFieldIn,
	    G4double            bGradIn,
	    G4double            kickAngle,
	    G4bool              verticalKickerIn,
	    BDSBeamPipeInfo*    beamPipeInfo,
	    BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSKicker(){;};
  
protected:
  virtual void Build();

private:
  G4double bField;
  G4double bGrad;
  G4double kickAngle;
  G4bool   verticalKicker;

  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe(); // overload multipole base class method

};

#endif
