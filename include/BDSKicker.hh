#ifndef BDSKicker_h
#define BDSKicker_h 

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "BDSDipoleStepper.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSSbendMagField.hh"

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

  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
};

#endif
