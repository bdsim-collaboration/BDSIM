#ifndef BDSSectorBend_h
#define BDSSectorBend_h 

#include "globals.hh"

#include "BDSMultipole.hh"

class BDSSectorBend :public BDSMultipole
{
public:
  BDSSectorBend(G4String aName, G4double aLength,
                G4double bpRad, G4double FeRad,
                G4double bField, G4double angle, G4double outR,
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                G4double tilt = 0, G4double bGrad=0, 
                G4String aTunnelMaterial="",
                G4String aMaterial = "", G4double xAper=0, G4double yAper=0);
  ~BDSSectorBend();

  virtual G4double GetChordLength();

private:
  G4double itsBField;
  G4double itsBGrad;
  /// chord length in [m]
  G4double itsChordLength;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildMarkerLogicalVolume();
  virtual void BuildBeampipe(G4String materialName = "");
  virtual void BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);

  virtual void SetVisAttributes();

  /// quad with poles and pockets
  void BuildStandardOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);
  /// cylinder
  void BuildCylindricalOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);

  /// calculate Tube Length for geometry building
  G4double CalculateTubeLength()const;

};

#endif
