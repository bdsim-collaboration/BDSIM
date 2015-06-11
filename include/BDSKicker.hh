#ifndef BDSKICKER_H
#define BDSKICKER_H 

#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSTiltOffset.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

class BDSKicker:public BDSMultipole
{
public:
  BDSKicker(G4String           name,
	    G4double           length,
	    G4double           bField,
	    G4double           bGrad,
	    G4double           angle,
	    G4bool             verticalKicker,
	    BDSBeamPipeInfo*   beamPipeInfo,
	    BDSMagnetOuterInfo magnetOuterInfo,
	    BDSTiltOffset      tiltOffset = BDSTiltOffset());
  ~BDSKicker(){;};
  
protected:
  virtual void Build();

private:
  G4double itsBField;
  G4double itsBGrad;
  G4double itsKickAngle;
  G4bool   isVerticalKicker;

  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe(); // overload multipole base class method

};

#endif
