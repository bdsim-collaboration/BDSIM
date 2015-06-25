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
  BDSKicker(G4String           name,
	    G4double           length,
	    G4double           bField,
	    G4double           bGrad,
	    G4double           angle,
	    G4bool             verticalKicker,
	    BDSBeamPipeInfo*   beamPipeInfo,
	    BDSMagnetOuterInfo magnetOuterInfo);
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
