#ifndef mySectorBend_h
#define mySectorBend_h 

#include "BDSMultipole.hh"
#include "globals.hh"

class myQuadStepper;
class BDSSbendMagField;
class G4Mag_EqRhs;
class G4VPhysicalVolume;
class G4VisAttributes;

class mySectorBend :public BDSMultipole
{
public:
  mySectorBend(G4String aName, G4double aLength,
	       G4double bpRad,G4double FeRad,
	       G4double bField, G4double angle, G4double outR,
	       G4double tilt = 0, G4double bGrad=0, G4String aMaterial = ""
               );
  ~mySectorBend();
  void SynchRescale(G4double factor);

protected:

private:
  G4double itsBField;
  G4double itsBGrad;
  G4VPhysicalVolume* itsPhysiInnerSB;
  G4VPhysicalVolume* itsPhysiCompSB;
  G4VPhysicalVolume* itsPhysiCompSBOuter;

  void BuildBPFieldAndStepper();
  void BuildSBMarkerLogicalVolume();
  void BuildSBBeampipe();
  void BuildSBOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);

  G4VisAttributes* SetVisAttributes();

  // field related objects:
  myQuadStepper* itsStepper;
  BDSSbendMagField* itsMagField;
  G4Mag_EqRhs* itsEqRhs;

};

#endif
