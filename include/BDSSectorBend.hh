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
  BDSSectorBend(G4String        nameIn,
		G4double        lengthIn,
		G4double        angleIn,
		G4double        bFieldIn,
		G4double        bGradIn,
		BDSBeamPipeType beamPipeType,
		G4double        aper1,
		G4double        aper2,
		G4double        aper3,
		G4double        aper4,
		G4Material*     vacuumMaterial,
		G4double        beamPipeThickness,
		G4Material*     beamPipeMaterial,
		G4double        boxSize,
		G4String        outerMaterial="",
		G4String        tunnelMaterial="",
		G4double        tunnelRadius=0,
		G4double        tunnelOffsetX=0);
  ~BDSSectorBend();

  virtual G4double GetChordLength();

private:
  G4double itsBField;
  G4double itsBGrad;
  
  /// chord length in [m]
  G4double itsChordLength;

  /// normal vectors for faces when preparing solids
  G4ThreeVector inputface;
  G4ThreeVector outputface;
  
  /// orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation;

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

};

#endif
