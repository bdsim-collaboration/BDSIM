#ifndef BDSKicker_h
#define BDSKicker_h 

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"
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
	    BDSBeamPipeInfo    beamPipeInfo,
	    G4double           boxSize,
	    G4String           outerMaterial="",
	    G4String           tunnelMaterial="",
	    G4double           tunnelRadius=0,
	    G4double           tunnelOffsetX=0);
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
  virtual void SetVisAttributes();
};

#endif
