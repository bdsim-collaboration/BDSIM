#ifndef BDSKicker_h
#define BDSKicker_h 

#include "globals.hh"
#include "BDSMultipole.hh"

class BDSKicker:public BDSMultipole
{
public:
  BDSKicker(G4String aName, G4double aLength,
	    G4double bpRad, G4double FeRad,
	    G4double bField, G4double angle, G4double outR,
	    G4double tilt = 0, G4double bGrad=0, 
            G4String aTunnelMaterial = "",
	    G4String aMaterial = "");
  ~BDSKicker();
  
protected:
  virtual void Build();

private:
  G4double itsBField;
  G4double itsBGrad;
  G4double itsKickAngle;

  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
};

#endif
